/* vi:ts=4:sw=4
 *
 * VIM - Vi IMitation
 *
 * Code Contributions By:	Bram Moolenaar			mool@oce.nl
 *							Tim Thompson			twitch!tjt
 *							Tony Andrews			onecom!wldrdg!tony 
 *							G. R. (Fred) Walter		watmath!watcgl!grwalter 
 */

/*
 * storage.c: allocation of lines and management of them
 *
 * part 1: storage allocation for the lines and blocks of the current file
 * part 2: managing of the pointer blocks
 *
 * Some code in this file depends on 68000 byte order!
 */

#include "vim.h"

/***************************************************************************
 * part 1: storage allocation for the lines and blocks of the current file *
 ***************************************************************************/

/*
 * Memory is allocated in relatively large blocks. These blocks are linked
 * in the allocated block list, headed by m_ahead. They are all freed
 * when abandoning a file.
 *
 * The available chunks of memory are kept in the free chunk list, headed
 * by m_fhead. This is a circular list. m_search points to the chunk before the
 * chunk that was freed/allocated the last time. alloc_line() gets a chunk from the
 * free list; free_line() returns a chunk to the free list.
 */

#define MEMBLOCKSIZE 2044		/* must not be multiple of 256! */

typedef struct m_info info_t;

/*
 * The m_info structure is at the start of each chunk of memory.
 * When the chunk is free the m_next field links it in the free chunk list.
 * When the chunk is in use the m_next field is used for data.
 */
struct m_info
{
	u_short  m_size;	/* size of the chunk (including m_info) */
	info_t	*m_next;	/* pointer to next free chunk in the list */
};

#define M_SIZESIZE sizeof(u_short)	/* size of m_size */

static char	 *m_ahead = NULL;		/* head of allocated memory block list */
static info_t m_fhead = {0, NULL};	/* head of free chunk list */
static info_t *m_search = NULL; 	/* pointer to chunk before previously
									   allocated/freed chunk */

#ifdef DEBUG
check_list()
{
	register info_t *mp;

	for (mp = &m_fhead; ; )
	{
		/*
		 * adjust these addresses for the actual available memory!
		 */
		if (mp >= 0x080000L && mp < 0x200000L || mp >= 0x400000L &&
			mp < 0xc00000 || mp >= 0xc80000 || mp->m_next->m_size > 23000)
		{
			m_error();
			return 1;
		}
		mp = mp->m_next;
		if (mp == &m_fhead)
			break;
	}
	return 0;
}

m_error()
{
	printf("list error\n");
}
#endif

/*
 * Allocate a block of memory and link it in the allocated list, so that
 * the block will be freed when abandoning the file.
 */
	char *
m_blockalloc(size, message)
	u_long size;
	bool_t message;
{
	char *p;

	p = lalloc(size + sizeof(char *), message);
	if (p != NULL)
	{
		*(char **)p = m_ahead;
		m_ahead = p;
		p += sizeof(char *);
	}
	return p;
}

/*
 * free all allocated memory blocks
 */
	void
m_blockfree()
{
	char *p, *np;

	for (p = m_ahead; p != NULL; p = np)
	{
		np = *(char **)p;
		free(p);
	}
	m_ahead = NULL;
	m_search = NULL;
}

/*
 * Free a chunk of memory which (probably) was allocated with alloc_line().
 * Insert the chunk into the free list, keeping it sorted on address.
 * CAREFUL: readfile() appends lines to the file, which do not have a
 * size field in front of them, but a single NUL character.
 */
	void
free_line(ptr)
	char *ptr;
{
	info_t			*mp;
	register info_t *next;
	info_t			*prev, *curr;
	long			len;

#ifdef DEBUG
	if (check_list())
		return;
#endif
	if (ptr == NULL || ptr == IObuff)	/* illegal address */
		return;
	if (*(ptr - 1) == NUL)		/* no size field */
	{
		len = strlen(ptr) + 1;
		if ((long)ptr & 1)		/* uneven pointers are not allowed; lose a byte */
		{
			++ptr;
			--len;
		}
		if (len <= (long)sizeof(info_t))	/* its too small, can't fit it in free list! */
			return;
		mp = (info_t *)ptr;
		mp->m_size = len;
	}
	else
		mp = (info_t *)(ptr - M_SIZESIZE);

	curr = NULL;
	/* if mp is smaller than m_search->m_next we start at m_fhead */
	if (mp < m_search->m_next)
		next = &m_fhead;
	else
		next = m_search;
	do
	{
		prev = curr;
		curr = next;
		next = next->m_next;
	}
	while (mp > next && next != &m_fhead);

/* if *mp and *next are concatenated, join them into one chunk */
	if ((char *)mp + mp->m_size == (char *)next)
	{
		mp->m_size += next->m_size;
		mp->m_next = next->m_next;
	}
	else
		mp->m_next = next;

/* if *curr and *mp are concatenated, join them */
	if (prev != NULL && (char *)curr + curr->m_size == (char *)mp)
	{
		curr->m_size += mp->m_size;
		curr->m_next = mp->m_next;
		m_search = prev;
	}
	else
	{
		curr->m_next = mp;
		m_search = curr;	/* put m_search before freed chunk */
	}
#ifdef DEBUG
	check_list();
#endif
}

/*
 * Allocate and initialize a new line structure with room for 'size'
 * characters.
 */
	char *
alloc_line(size)
	register unsigned size;
{
	register info_t *mp, *mprev;

#ifdef DEBUG
	if (m_search != NULL && check_list())
		return NULL;
#endif
/*
 * add size field and room for trailing NUL byte
 * make even and adjust for minimal size (must be able to store size, pointer
 * plus a trailing NUL)
 */
	size = (size + M_SIZESIZE + 2) & ~1;
	if (size < sizeof(info_t) + 2)
		size = sizeof(info_t) + 2;

/* make sure that the byte in front of the line is not NUL */
	if ((size & 0xff) == NUL)
		size += 2;

/* if m_search is NULL we have to initialize the free list */
	if (m_search == NULL)
	{
		m_search = &m_fhead;
		m_fhead.m_next = &m_fhead;
	}

/* search for space in free list */
	mprev = m_search;
	for (mp = m_search->m_next; mp->m_size < size; mp = mp->m_next)
	{
		if (mp == m_search)
		{
			int		n = (size > (MEMBLOCKSIZE / 4) ? size : MEMBLOCKSIZE);

			mp = (info_t *)m_blockalloc((u_long)n, (bool_t)TRUE);
			if (mp == NULL)
				return (NULL);
			mp->m_size = n;
			free_line((char *)mp + M_SIZESIZE);
			mp = m_search;
		}
		mprev = mp;
	}

/* if the chunk we found is large enough, split it up in two */
	if (mp->m_size - size >= sizeof(info_t))
	{
		if (mp->m_size & 1)				/* remaining size must be even */
			++size;
		mp->m_size -= size;
		mp = (info_t *)((char *)mp + mp->m_size);
		mp->m_size = size;
	}
	else								/* remove *mp from the free list */
	{
		mprev->m_next = mp->m_next;
	}
	m_search = mprev;

	*((char *)mp + size - 1) = NUL;		/* make sure the last byte is a NUL */
	mp = (info_t *)((char *)mp + M_SIZESIZE);
	*(char *)mp = NUL;					/* set the first byte to NUL */
#ifdef DEBUG
	check_list();
#endif

	return ((char *)mp);
}

/*
 * save_line(): allocate memory with alloc_line() and copy the string 'src' into it.
 */
	char *
save_line(src)
	register char *src;
{
	register char *dst;
	register unsigned len;

	len = strlen(src);
	if ((dst = alloc_line(len)) != NULL)
		movmem(src, dst, (size_t)(len + 1));
	return (dst);
}

/******************************************
 * part 2: managing of the pointer blocks *
 ******************************************/

typedef struct block block_t;

#define BLOCK_SIZE 40

struct block
{
	u_short  b_count;				/* current number of pointers in b_ptr */
	block_t *b_next;				/* pointer to next block */
	block_t *b_prev;				/* pointer to previous block */
	char	*b_ptr[BLOCK_SIZE];		/* pointers to the lines */
	char	 b_flags[BLOCK_SIZE];	/* see below */
};

#define B_MARKED	0x01		/* mark for :global command */

static block_t *first_block;	/* pointer to first block in block list */
static block_t *last_block;		/* pointer to last block in block list */

static block_t *curr_block;		/* block used by nr2ptr */
static linenr_t curr_count;		/* first line number of block curr_block */

static block_t *alloc_block __ARGS((void));

    static block_t *
alloc_block()
{
	block_t *p;

	p = (block_t *)alloc_line((unsigned)sizeof(block_t));
	if (p != NULL)
	{
		memset(p, 0, sizeof(block_t));
	}
	return (p);
}

/*
 * filealloc() - construct an initial empty file buffer
 */
	void
filealloc()
{
	first_block = last_block = alloc_block();
	if (first_block == NULL || (first_block->b_ptr[0] = alloc_line(0)) == NULL)
		getout(1);
	first_block->b_count = 1;
	Curpos.lnum = 1;
	Curswant = Curpos.col = 0;
	Topline = 1;
	line_count = 1;
	curr_count = 0;
	clrallmarks();
	clrtags();
}

/*
 * freeall() - free the current buffer
 *
 * Free all lines in the current buffer.
 */
	void
freeall()
{
	m_blockfree();
	line_count = 0;
	s_ins(0, 0, (bool_t)TRUE);	/* invalidate Line arrays */
	u_clearall();
}

/*
 * Get the pointer to the line 'nr'.
 * This function is used a lot for sequential access (writeit, search),
 * so that is what it is optimized for.
 */
    char *
nr2ptr(nr)
    register linenr_t nr;
{
	register linenr_t count;
	register block_t *bp;

	if ((count = curr_count) == 0 || nr >= count + (bp = curr_block)->b_count || nr < count)
	{
		if (nr < 1 || nr > line_count)
		{
			emsg("nr2ptr: illegal nr");
			return (IObuff);	/* always return a valid ptr */
		}

		/*
		 * three ways to find the pointer:
		 * 1. first pointer in the next block (fast for sequential access)
		 * 2. search forward
		 * 3. search backward
		 */
		if (count && nr == count + bp->b_count)		/* in next block */
		{
			count = nr;
			bp = bp->b_next;
		}
		else if (nr <= (count + line_count) / 2 || nr <= count && nr <= count / 2)
		{
													/* search forward */
			if (nr < count || count == 0)
			{
				count = 1;
				bp = first_block;
			}
			while (bp != NULL)
			{
				count += bp->b_count;
				if (nr < count)
				{
					count -= bp->b_count;
					break;
				}
				bp = bp->b_next;
			}
		}
		else
		{											/* search backward */
			if (nr < count)
				bp = bp->b_prev;
			else
			{
				bp = last_block;
				count = line_count + 1;
			}
			while (bp != NULL)
			{
				count -= bp->b_count;
				if (nr >= count)
					break;
				bp = bp->b_prev;
			}
		}
		
		if (bp == NULL)
		{
			emsg("nr2ptr: strorage corrupt");
			curr_count = 0;
			return (IObuff);
		}
		curr_count = count;
		curr_block = bp;
	}
	return (bp->b_ptr[nr - count]);
}

/*
 * set the B_MARKED flag for line 'lnum'
 */
	void
setmarked(lnum)
	linenr_t lnum;
{
	nr2ptr(lnum);
	curr_block->b_flags[lnum - curr_count] |= B_MARKED;
}

/*
 * find the first line with its B_MARKED flag set
 */
	linenr_t
firstmarked()
{
	register block_t	*bp;
	register linenr_t	lnum;
	register int		i;

	for (bp = first_block, lnum = 1; bp != NULL; bp = bp->b_next)
		for (i = 0; i < bp->b_count; ++i, ++lnum)
			if (bp->b_flags[i] & B_MARKED)
			{
				bp->b_flags[i] &= ~B_MARKED;
				return lnum;
			}
	return (linenr_t) 0;
}

/*
 * clear all B_MARKED flags
 */
	void
clearmarked()
{
	register block_t	*bp;
	register int		i;

	for (bp = first_block; bp != NULL; bp = bp->b_next)
		for (i = bp->b_count; --i >= 0; )
				bp->b_flags[i] &= ~B_MARKED;
}

/*
 * a pointer to a line is converted into a line number
 * we start at line number 'start'
 * this is a bit slow, but it is used for marks and undo only
 */
	linenr_t
ptr2nr(ptr, start)
	char *ptr;
	linenr_t start;
{
	block_t *bp;
	register linenr_t nr;
	register char **pp;
	register int	i;

	if (ptr == NULL)
		return (linenr_t)0;

	if (start == 0)
	    start = 1;
	nr2ptr(start);	    /* set curr_block and curr_count */

	for (nr = curr_count, bp = curr_block; bp != NULL; bp = bp->b_next)
		for (pp = bp->b_ptr, i = bp->b_count; --i >= 0; ++nr)
			if (*pp++ == ptr)
				return (nr);
	return (linenr_t)0;
}

/*
 * appendline: add a line
 *	return TRUE when succesful
 */
	bool_t
appendline(after, s)
	linenr_t	after;
	char		*s;
{
	register block_t	*bp;
	block_t 			*nbp;
	linenr_t			count;
	register int		i;

	if (s == NULL)		/* don't insert NULL pointers! */
		return FALSE;
	if (after == 0) 	/* insert in front of first line */
	{
		bp = first_block;
		count = 1;
		if (bufempty()) 	/* simply replace dummy line */
		{
			free_line(bp->b_ptr[0]);
			bp->b_ptr[0] = s;
			return TRUE;
		}
		curr_count = 0; /* curr_block will become invalid */
	}
	else
	{
		(void)nr2ptr(after);	/* find block */
		bp = curr_block;
		count = curr_count;
	}

	++line_count;
	i = bp->b_count;
	if (i < BLOCK_SIZE)			/* there is place in the current block */
/* move ptrs one place forward to make space for new one */
	{
		register char **pp;
		register char *fp;

		pp = &(bp->b_ptr[i]);
		fp = &(bp->b_flags[i]);
		for (i += count - after - 1; --i >= 0; --pp, --fp)
		{
			*pp = *(pp - 1);
			*fp = *(fp - 1);
		}
		*pp = s;
		*fp = 0;
		++bp->b_count;
		return TRUE;
	}

/* need to allocate a new block */
	nbp = alloc_block();
	if (nbp == NULL)
	{
		--line_count;
		free_line(s);
		return FALSE;
	}

/* put new block in linked list */
	if (after == 0) /* put new block in front of linked list */
	{
		bp->b_prev = nbp;
		nbp->b_next = bp;
		first_block = nbp;
		nbp->b_ptr[0] = s;
		nbp->b_count = 1;
		return TRUE;
	}

	/* insert new block in linked list after bp */
	nbp->b_next = bp->b_next;
	bp->b_next = nbp;
	nbp->b_prev = bp;
	if (nbp->b_next == NULL)
		last_block = nbp;
	else
		nbp->b_next->b_prev = nbp;

	if (after - count + 1 == BLOCK_SIZE) /* put s in new block */
	{
		nbp->b_ptr[0] = s;
		nbp->b_count = 1;
		return TRUE;
	}

	/* move some ptrs from full block to new block */
	{
		register int j;

		bp->b_count = after - count + 1;	/* number of ptrs remaining */
		i = BLOCK_SIZE - bp->b_count;		/* number of ptrs to be moved */
		nbp->b_count = i;
		while (--i >= 0)
		{
			j = bp->b_count + i;
			nbp->b_ptr[i] = bp->b_ptr[j];
			nbp->b_flags[i] = bp->b_flags[j];
		}
		bp->b_ptr[j] = s;
		bp->b_flags[j] = 0;
		++bp->b_count;
		return TRUE;
	}
}

/*
 * delsline: delete line from storage
 *
 * the line is turned over to the caller
 */
	char *
delsline(nr)
	linenr_t nr;
{
	char	*ptr;
	register block_t	*bp;
	register char **pp;
	register char *fp;
	register int i;

	if (nr < 1 || nr > line_count)
	{
		emsg("delsline: nr wrong");
		return (alloc_line(0));
	}
	ptr = nr2ptr(nr);
	adjustmark(ptr, NULL);			/* remove marks for this line */
	bp = curr_block;
	if (line_count == 1)	/* don't delete the last line in the file */
	{
		bp->b_ptr[0] = alloc_line(0);
		return (ptr);
	}
	--line_count;

	/* move the rest of the ptrs in this block one down */
	pp = &(bp->b_ptr[nr - curr_count]);
	fp = &(bp->b_flags[nr - curr_count]);
	for (i = bp->b_count + curr_count - nr - 1; --i >= 0; ++pp, ++fp)
	{
		*pp = *(pp + 1);
		*fp = *(fp + 1);
	}
	if (--bp->b_count == 0) /* the block became empty, remove it from the list */
	{
		if (bp->b_prev == NULL)
			first_block = bp->b_next;
		else
			bp->b_prev->b_next = bp->b_next;
		if (bp->b_next == NULL)
			last_block = bp->b_prev;
		else
			bp->b_next->b_prev = bp->b_prev;
		free_line((char *)bp);
		curr_count = 0; /* curr_block invalid */
	}
	return (ptr);
}

/*
 * replace the line "lnum" with the line "new".
 * return the old line (which should be freed by the caller)
 */
	char *
replaceline(lnum, new)
	linenr_t lnum;
	char *new;
{
	char *old;

	old = nr2ptr(lnum);
	if (new == NULL || curr_count == 0)	/* we don't want NULL pointers in the list */
		return (alloc_line(0)); /* be friendly to the caller */

	curr_block->b_ptr[lnum - curr_count] = new;
	curr_block->b_flags[lnum - curr_count] = 0;
	adjustmark(old, new);
	return (old);
}

/*
 * canincrease(n) - returns TRUE if the current line can be increased 'n'
 * bytes
 *
 * This routine returns immediately if the requested space is available. If not,
 * it attempts to allocate the space and adjust the data structures
 * accordingly. If everything fails it returns FALSE.
 */
	bool_t
canincrease(n)
    int    n;
{
	register char	*old;
    register char	*new;		/* pointer to new space */
    register unsigned newsize;

	old = nr2ptr(Curpos.lnum);
    newsize = strlen(old) + n;

    new = alloc_line(newsize);
    if (new == NULL)
		return FALSE;

    strcpy(new, old);
	adjustmark(old, new);
    free_line(old);
    curr_block->b_ptr[Curpos.lnum - curr_count] = new;
    curr_block->b_flags[Curpos.lnum - curr_count] = 0;

    return TRUE;
}
