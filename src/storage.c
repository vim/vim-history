/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved
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
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"

/***************************************************************************
 * part 1: storage allocation for the lines and blocks of the current file *
 ***************************************************************************/

/*
 * Memory is allocated in relatively large blocks. These blocks are linked
 * in the allocated block list, headed by block_head. They are all freed
 * when abandoning a file, so we don't have to free every single line. The
 * list is kept sorted on memory address.
 * block_alloc() allocates a block.
 * m_blockfree() frees all blocks.
 *
 * The available chunks of memory are kept in free chunk lists. There is
 * one free list for each block of allocated memory. The list is kept sorted
 * on memory address.
 * alloc_line() gets a chunk from the free lists.
 * free_line() returns a chunk to the free lists.
 * m_search points to the chunk before the chunk that was freed/allocated the
 * last time.
 * mb_current points to the b_head where m_search points into the free list.
 *
 *
 *    block_head     /---> block #1     /---> block #2
 *       mb_next ---/       mb_next ---/       mb_next ---> NULL
 *       mb_info            mb_info            mb_info
 *          |                  |                  |
 *          V                  V                  V
 *        NULL          free chunk #1.1      free chunk #2.1
 *                             |                  |
 *                             V                  V
 *                      free chunk #1.2          NULL
 *                             |
 *                             V
 *                            NULL
 *
 * When a single free chunk list would have been used, it could take a lot
 * of time in free_line() to find the correct place to insert a chunk in the
 * free list. The single free list would become very long when many lines are
 * changed (e.g. with :%s/^M$//).
 */

	/*
	 * this blocksize is used when allocating new lines
	 * When reading files larger blocks are used (see fileio.c)
	 * on the Amiga the blocksize must not be a multiple of 256
	 * with MS-Dos the blocksize must be larger than 255
	 * For Unix it does not really matter
	 */
#define MEMBLOCKSIZE 2044

typedef struct m_info info_t;

/*
 * There are two types of in-use memory chunks:
 *	1. those that are allocated by readfile(). These are always preceded
 *		by a NUL character and end in a NUL character. The chunk must not
 *		contain other NUL characters. The preceding NUL is used to
 *		determine the chunk type. The ending NUL is used to determine the
 *		end of the chunk. The preceding NUL is not part of the chunk, the
 *		ending NUL is. 
 *  2. the other chunks have been allocated with alloc_line(). They are
 *		preceded by a non-NUL character. This is used to determine the chunk
 *		type. The non-NUL may be part of a size field or may be an extra 0xff
 *		byte. The chunk always ends in a NUL character and may also contain
 *		a NUL character. The size field contains the size of the chunk,
 *		including the size field. The trailing NUL may be used by a possibly
 *		follwing type 1 chunk. The preceding size, the optional 0xff and the
 *		trailing NUL are all part of the chunk.
 *
 * When the chunk is not in-use it is preceded with the m_info structure.
 * The m_next field links it in one of the free chunk lists. It must still
 * end in a NUL byte, because it may be followed by a type 1 chunk!
 *
 * When in-use we must make sure there is a non-NUL byte in front of type
 * 2 chunks.
 *
 * On the Amiga this means that the size must not be a multiple of 256.
 * This is done by multiplying the size by 2 and adding one.
 *
 * On MS-DOS the size must be larger than 255. This is done by adding 256
 * to the size.
 *
 * On Unix systems an extra 0xff byte is added. This costs 4 bytes because
 * pointers must be kept long-aligned.
 *
 * On most unix systems structures have to be longword (32 or 64 bit) aligned.
 * On most other systems they are short (16 bit) aligned.
 */

#ifdef UNIX
# define ALIGN_LONG		/* longword alignment and use filler byte */
# define ALIGN_SIZE (sizeof(long))
# define ALIGN_MASK (ALIGN_SIZE - 1)
#else
# ifdef AMIGA
#  define LOWBYTE		/* size must not be multiple of 256 */
# else
#  ifdef MSDOS
#   define HIGHBYTE		/* size must be larger than 255 */
#  else
	you must define something!
#  endif
# endif
#endif

/*
 * stucture used to link chunks in one of the free chunk lists.
 */
struct m_info
{
#ifdef ALIGN_LONG
	u_long	 m_size;	/* size of the chunk (including m_info) */
#else
	u_short  m_size;	/* size of the chunk (including m_info) */
#endif
	info_t	*m_next;	/* pointer to next free chunk in the list */
};

#ifdef ALIGN_LONG
	/* size of m_size + room for 0xff byte */
# define M_OFFSET (sizeof(u_long) * 2)
#else
	/* size of m_size */
# define M_OFFSET (sizeof(u_short))
#endif

/*
 * structure used to link blocks in the list of allocated blocks.
 */
struct m_block
{
	struct m_block	*mb_next;	/* pointer to next allocated block */
	info_t			mb_info;	/* head of free chuck list for this block */
};

static struct m_block block_head = {NULL, {0, NULL}};
								/* head of allocated memory block list */

static info_t *m_search = NULL; 	/* pointer to chunk before previously
									   allocated/freed chunk */
static struct m_block *mb_current = NULL;	/* block where m_search points in */

/*
 * Allocate a block of memory and link it in the allocated block list.
 */
	char *
m_blockalloc(size, message)
	u_long	size;
	int		message;
{
	struct m_block *p;
	struct m_block *mp, *next;

	p = (struct m_block *)lalloc(size + sizeof(struct m_block), message);
	if (p != NULL)
	{
		 /* Insert the block into the allocated block list, keeping it
		 			sorted on address. */
		for (mp = &block_head; (next = mp->mb_next) != NULL && next < p; mp = next)
			;
		p->mb_next = next;				/* link in block list */
		mp->mb_next = p;
		p->mb_info.m_next = NULL;		/* clear free list */
		p->mb_info.m_size = 0;
		mb_current = p;					/* remember current block */
		m_search = NULL;
		p++;							/* return usable memory */
	}
	return (char *)p;
}

/*
 * free all allocated memory blocks
 */
	void
m_blockfree()
{
	struct m_block *p, *np;

	for (p = block_head.mb_next; p != NULL; p = np)
	{
		np = p->mb_next;
		free((char *)p);
	}
	block_head.mb_next = NULL;
	m_search = NULL;
	mb_current = NULL;
}

/*
 * Free a chunk of memory which was
 *  1. inserted with readfile(); these are preceded with a NUL byte
 *  2. allocated with alloc_line(); these are preceded with a non-NUL byte
 * Insert the chunk into the correct free list, keeping it sorted on address.
 */
	void
free_line(ptr)
	char *ptr;
{
	register info_t		*next;
	register info_t		*prev, *curr;
	register info_t		*mp;
	long				len;
	struct m_block		*nextb;

	if (ptr == NULL || ptr == IObuff)
		return;	/* illegal address can happen in out-of-memory situations */

	if (*(ptr - 1) == NUL)		/* type 1 chunk: no size field */
	{
#ifdef ALIGN_LONG		/* use longword alignment */
		long c;

		len = strlen(ptr) + 1;
		if ((c = ((long)ptr & ALIGN_MASK)) != 0)     /* lose some bytes */
		{
		    c = ALIGN_SIZE - c;
		    ptr += c;
		    len -= c;
		}
#else			/* use short (16 bit) alignment */
		len = strlen(ptr) + 1;
		if (len > 0xff00)		/* can't handle this large (cannot happen?) */
			len = 0xff00;
		if ((long)ptr & 1)					/* lose a byte */
		{
			++ptr;
			--len;
		}
#endif	/* ALIGN_LONG */

		/* we must be able to store size, pointer and a trailing NUL */
		/* otherwise we can't fit it in the free list */
		if (len <= (long)sizeof(info_t))
			return;			/* these bytes are not used until you quit the file */
		mp = (info_t *)ptr;
		mp->m_size = len;
	}
#ifdef ALIGN_LONG
	else if ((*(ptr - 1) & 0xff) == 0xff)	/* type 2 chunk: has size field */
	{
		mp = (info_t *)(ptr - M_OFFSET);
	}
	else		/* illegal situation: there is no NUL or 0xff in front of the line */
	{
		emsg("Illegal chunk");
		return;
	}
#endif
#ifdef LOWBYTE
	else 			/* type 2 chunk: has size field */
	{
		mp = (info_t *)(ptr - M_OFFSET);
		mp->m_size >>= 1;
	}
#endif
#ifdef HIGHBYTE
	else 			/* type 2 chunk: has size field */
	{
		mp = (info_t *)(ptr - M_OFFSET);
		mp->m_size -= 256;
	}
#endif

		/* find block where chunk could be a part off */
		/* if we change mb_current, m_search is set to NULL */
	if (mb_current == NULL || mp < (info_t *)mb_current)
	{
		mb_current = block_head.mb_next;
		m_search = NULL;
	}
	if ((nextb = mb_current->mb_next) != NULL && (info_t *)nextb < mp)
	{
		mb_current = nextb;
		m_search = NULL;
	}
	while ((nextb = mb_current->mb_next) != NULL && (info_t *)nextb < mp)
		mb_current = nextb;

	curr = NULL;
	/* if mp is smaller than m_search->m_next we go to the start of the free list */
	if (m_search == NULL || mp < (m_search->m_next))
		next = &(mb_current->mb_info);
	else
		next = m_search;
	/*
	 * The following loop is executed very often.
	 * Therefore it has been optimized at the cost of readability.
	 * Keep it fast!
	 */
#ifdef SLOW_BUT_EASY_TO_READ
	do
	{
		prev = curr;
		curr = next;
		next = next->m_next;
	}
	while (mp > next && next != NULL);
#else
	do										/* first, middle, last */
	{
		prev = next->m_next;				/* curr, next, prev */
		if (prev == NULL || mp <= prev)
		{
			prev = curr;
			curr = next;
			next = next->m_next;
			break;
		}
		curr = prev->m_next;				/* next, prev, curr */
		if (curr == NULL || mp <= curr)
		{
			prev = next;
			curr = prev->m_next;
			next = curr->m_next;
			break;
		}
		next = curr->m_next;				/* prev, curr, next */
	}
	while (mp > next && next != NULL);
#endif

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
}

/*
 * Allocate and initialize a new line structure with room for at least
 * 'size' characters.
 */
	char *
alloc_line(size)
	register unsigned size;
{
	register info_t *mp, *mprev, *mp2;
	struct m_block	*mbp;
	int		 		size_align;

/*
 * Add room for size field, optional 0xff byte and trailing NUL byte.
 * Adjust for minimal size (must be able to store info_t
 * plus a trailing NUL, so the chunk can be released again)
 */
	size += M_OFFSET + 1;
	if (size < sizeof(info_t) + 1)
	  size = sizeof(info_t) + 1;

/*
 * round size up for alignment
 */
#ifdef ALIGN_LONG			/* use longword alignment */
	size_align = (size + ALIGN_MASK) & ~ALIGN_MASK;
#else /* ALIGN_LONG */	/* use short (16 bit) alignment */
	size_align = (size + 1) & ~1;
#endif	/* ALIGN_LONG */

/* if m_search is NULL (uninitialized free list) we start at block_head */
	if (mb_current == NULL || m_search == NULL)
	{
		mb_current = &block_head;
		m_search = &(block_head.mb_info);
	}

/* search for space in free list */
	mprev = m_search;
	mbp = mb_current;
	mp = m_search->m_next;
	if (mp == NULL)
	{
		if (mbp->mb_next)
			mbp = mbp->mb_next;
		else
			mbp = &block_head;
		mp = m_search = &(mbp->mb_info);
	}
	while (mp->m_size < size)
	{
		if (mp == m_search)		/* back where we started in free chunk list */
		{
			if (mbp->mb_next)
				mbp = mbp->mb_next;
			else
				mbp = &block_head;
			mp = m_search = &(mbp->mb_info);
			if (mbp == mb_current)	/* back where we started in block list */
			{
				int		n = (size_align > (MEMBLOCKSIZE / 4) ? size_align : MEMBLOCKSIZE);

				mp = (info_t *)m_blockalloc((u_long)n, TRUE);
				if (mp == NULL)
					return (NULL);
#ifdef HIGHBYTE
				mp->m_size = n + 256;
#endif
#ifdef LOWBYTE
				mp->m_size = (n << 1) + 1;
#endif
#ifdef ALIGN_LONG
				mp->m_size = n;
				*((u_char *)mp + M_OFFSET - 1) = 0xff;
#endif
				free_line((char *)mp + M_OFFSET);
				mp = m_search;
				mbp = mb_current;
			}
		}
		mprev = mp;
		if ((mp = mp->m_next) == NULL)		/* at end of the list */
			mp = &(mbp->mb_info);			/* wrap around to begin */
	}

/* if the chunk we found is large enough, split it up in two */
	if ((long)mp->m_size - size_align >= (long)(sizeof(info_t) + 1))
	{
		mp2 = (info_t *)((char *)mp + size_align);
		mp2->m_size = mp->m_size - size_align;
		mp2->m_next = mp->m_next;
		mprev->m_next = mp2;
		mp->m_size = size_align;
	}
	else					/* remove *mp from the free list */
	{
		mprev->m_next = mp->m_next;
	}
	m_search = mprev;
	mb_current = mbp;

#ifdef HIGHBYTE
	mp->m_size += 256;
#endif
#ifdef LOWBYTE
	mp->m_size = (mp->m_size << 1) + 1;
#endif
	mp = (info_t *)((char *)mp + M_OFFSET);
#ifdef ALIGN_LONG
	*((u_char *)mp - 1) = 0xff;			/* mark type 2 chunk */
#endif
	*(char *)mp = NUL;					/* set the first byte to NUL */

	return ((char *)mp);
}

/*
 * save_line(): allocate memory with alloc_line() and copy the
 * string 'src' into it.
 */
	char *
save_line(src)
	register char *src;
{
	register char *dst;
	register unsigned len;

	len = strlen(src);
	if ((dst = alloc_line(len)) != NULL)
		memmove(dst, src, (size_t)(len + 1));
	return (dst);
}

/******************************************
 * part 2: managing of the pointer blocks *
 ******************************************/

typedef struct block block_t;

#ifdef BLOCK_SIZE
# undef BLOCK_SIZE	/* for Linux: is in limits.h */
#endif

#define BLOCK_SIZE 40

struct block
{
	char	*b_ptr[BLOCK_SIZE];		/* pointers to the lines */
	char	 b_flags[BLOCK_SIZE];	/* see below */
	u_short  b_count;				/* current number of pointers in b_ptr */
	block_t *b_next;				/* pointer to next block */
	block_t *b_prev;				/* pointer to previous block */
};

#define B_MARKED	0x01		/* mark for :global command */

static block_t *first_block;	/* pointer to first block in block list */
static block_t *last_block;		/* pointer to last block in block list */

static block_t *curr_block;		/* block used by nr2ptr */
static linenr_t curr_count;		/* first line number of block curr_block */
static linenr_t curr_count_max;	/* curr_count + curr_block->b_count */

static block_t *alloc_block __ARGS((void));

    static block_t *
alloc_block()
{
	block_t *p;

	p = (block_t *)(alloc_line((unsigned)sizeof(block_t)));
	if (p != NULL)
	{
		memset((char *)p, 0, sizeof(block_t));
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
	Botline = 2;
	line_count = 1;
	curr_count = 0;
	clrallmarks();
	clrtags();
	UNCHANGED;
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
	s_ins(0, 0, TRUE);	/* invalidate Line arrays */
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
	register linenr_t count = curr_count;

	/*
	 * if we don't have a current block or the line is not in the current block,
	 * make the block containing the line the current block.
	 */
	if (count == 0 || nr >= curr_count_max || nr < count)
	{
		register block_t *bp = curr_block;

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
		else if (nr <= (count + line_count) / 2 ||
				(nr <= count && nr <= count / 2))
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
		curr_count_max = count + bp->b_count;
		curr_block = bp;
	}
	return (curr_block->b_ptr[nr - count]);
}

/*
 * pos2ptr: get pointer to position 'pos'
 */
    char *
pos2ptr(pos)
    FPOS	*pos;
{
	return (nr2ptr(pos->lnum) + pos->col);
}

	char *
Curpos2ptr()
{
	return (nr2ptr(Curpos.lnum) + Curpos.col);
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
	register u_short	i;

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
	int
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
		++curr_count_max;
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
		register int j = 0;

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
		curr_count_max = curr_count + bp->b_count;
	}
	return TRUE;
}

/*
 * delsline: delete line from storage
 *
 * the line is turned over to the caller
 */
	char *
delsline(nr, delmarks)
	linenr_t	nr;
	int			delmarks;
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
	if (delmarks)
		adjustmark(ptr, NULL);			/* remove marks for this line (slow!) */
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
	else
		--curr_count_max;
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
	int
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
