/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 * VMS port			by Henk Elbers
 * VMS deport                   by Zoltan Arpadffy
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include	"vim.h"

#define EFN             0                       /* Event flag */
#define NOTIM	        0
#define TIM_0	        1
#define TIM_1	        2
#define EXPL_ALLOC_INC 16

typedef struct
{
    char	class;
    char	type;
    short	width;
    union
    {
	struct
	{
	    char	_basic[3];
	    char	length;
	}	y;
	int	basic;
    }	x;
    int		extended;
}	TT_MODE;

typedef struct
{
    short	buflen;
    short	itemcode;
    char	*bufadrs;
    int		*retlen;
}	ITEM;

typedef struct
{
    ITEM	equ;
    int		nul;
}	ITMLST1;

typedef struct
{
    ITEM	index;
    ITEM	string;
    int	nul;
}	ITMLST2;

char            ibuf[1];                /* Input buffer */
static TT_MODE  orgmode;
static short    iochan;                 /* TTY I/O channel */
static short    iosb[4];                /* IO status block */

int vms_match_num = 0;
int vms_match_alloced = 0;
int vms_match_free = 0;
char_u **vms_fmatch = NULL;
static char *Fspec_Rms;                /* rms file spec, passed implicitly between routines */

static TT_MODE	get_tty __ARGS((void));
static void	set_tty __ARGS((int row, int col));
static int	vms_wait __ARGS((unsigned int fun));

#define EQN(S1,S2,LN) (strncmp(S1,S2,LN) == 0)
#define SKIP_FOLLOWING_SLASHES(Str) while (Str[1] == '/') ++Str

/*
 * low level input funcion.
 *
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    int
mch_inchar(char_u *buf, int maxlen, long wtime)
{
    int		c, res;

#ifdef FEAT_GUI
    if (gui.in_use)
    {
	if (!gui_wait_for_chars(wtime))
	    return 0;
	return read_from_input_buf(buf, (long)maxlen);
    }
#endif
    /* first check to see if any characters read by
     * mch_breakcheck(), mch_delay() or mch_char_avail()
     */
    if (!vim_is_input_buf_empty())
	return read_from_input_buf(buf, (long)maxlen);

    out_flush();
    if (wtime == -1)
	res = vms_wait(NOTIM);			/* without timeout */
    else
	res = vms_wait(wtime ? TIM_1 : TIM_0);	/* with timeout */
    if (res != SS$_NORMAL)
	return(0);
    out_flush();
    c = (ibuf[0] & 0xFF);			/* Allow multinational */
    *buf = c;
    return(1);
}

    static int
vms_wait(unsigned int fun)
{
    int			ret, term[2] = { 0, 0 };

    switch(fun)
    {
	case NOTIM:		/* wait forever for characters		*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK, iosb, 0, 0,
			   ibuf, 1, 0, term, 0, 0);
	    break;
	case TIM_1:		/* wait a short time for characters	*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
			   ibuf, 1, 1, term, 0, 0);
	    break;
	default:		/* do not wait for characters		*/
	    ret = sys$qiow(EFN, iochan, IO$_READLBLK | IO$M_TIMED, iosb, 0, 0,
			   ibuf, 1, 0, term, 0, 0);
	    break;
    }
    if (ret != SS$_NORMAL)
	return(ret);
    if ((fun && (iosb[0] != SS$_TIMEOUT) && (iosb[0] != SS$_NORMAL)) ||
	(!fun && (iosb[0] != SS$_NORMAL)))
	return(iosb[0]);
    return(iosb[1] + iosb[3]);
}

/*
 *	vul_desc	vult een descriptor met een string en de lengte
 *			hier van.
 */
    static void
vul_desc(DESC *des, char *str)
{
    des->dsc$b_dtype = DSC$K_DTYPE_T;
    des->dsc$b_class = DSC$K_CLASS_S;
    des->dsc$a_pointer = str;
    des->dsc$w_length = str ? strlen(str) : 0;
}

/*
 *	vul_item	vult een item met een aantal waarden
 */
    static void
vul_item(ITEM *itm, short len, short cod, char *adr, int *ret)
{
    itm->buflen   = len;
    itm->itemcode = cod;
    itm->bufadrs  = adr;
    itm->retlen   = ret;
}

    void
mch_settmode(int tmode)
{
    long        mystatus;

    if ( tmode == TMODE_RAW )
	set_tty(0, 0);
    else{
        switch (orgmode.width)
        {
	    case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	    case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	    default:	break;
        }
        out_flush();
        mystatus = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			&orgmode, sizeof(TT_MODE), 0,0,0,0);
        if (mystatus!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	    return;
        (void)sys$dassgn(iochan);
        iochan = 0;
    }
}

    static void
set_tty(int row, int col)
{
    long	    mystatus;
    TT_MODE	    newmode;		/* New TTY mode bits		*/
    static short first_time = TRUE;

    if (first_time)
    {
	orgmode = get_tty();
	first_time = FALSE;
    }
    newmode = get_tty();
    if (col)
	newmode.width            = col;
    if (row)
	newmode.x.y.length       = row;
    newmode.x.basic		|= (TT$M_NOECHO | TT$M_HOSTSYNC);
    newmode.x.basic		&= ~TT$M_TTSYNC;
    newmode.extended		|= TT2$M_PASTHRU;
    mystatus = sys$qiow(0, iochan, IO$_SETMODE, iosb, 0, 0,
			  &newmode, sizeof(newmode), 0, 0, 0, 0);
    if (mystatus!=SS$_NORMAL || (iosb[0]&0xFFFF)!=SS$_NORMAL)
	return;
}

    static TT_MODE
get_tty(void)
{
    long	mystatus;
    TT_MODE	tt_mode;
    DESC	odsc;
    char	*cp1, *cp2 = "TT";

    if (!iochan)
    {
        while (cp1 = (char *)mch_getenv((char_u *)cp2))
	    cp2 = cp1;
        vul_desc(&odsc, cp2);
        (void)sys$assign(&odsc, &iochan, 0, 0);
    }
    mystatus = sys$qiow(0, iochan, IO$_SENSEMODE, iosb, 0, 0,
		      &tt_mode, sizeof(tt_mode), 0, 0, 0, 0);
    if (mystatus != SS$_NORMAL || (iosb[0] & 0xFFFF) != SS$_NORMAL)
    {
	tt_mode.width		= 0;
	tt_mode.type		= 0;
	tt_mode.class		= 0;
	tt_mode.x.basic		= 0;
	tt_mode.x.y.length	= 0;
	tt_mode.extended	= 0;
    }
    return(tt_mode);
}

/*
 * Get the current window size in Rows and Columns.
 */
    int
mch_get_shellsize(void)
{
    TT_MODE	tmode;

    tmode = get_tty();			/* get size from VMS	*/
    Columns = tmode.width;
    Rows = tmode.x.y.length;
    return OK;
}

/*
 * Try to set the window size to Rows and new_Columns.
 */
    void
mch_set_shellsize(void)
{
    set_tty(Rows, Columns);
    switch (Columns)
    {
	case 132:	OUT_STR_NF((char_u *)"\033[?3h\033>");	break;
	case 80:	OUT_STR_NF((char_u *)"\033[?3l\033>");	break;
	default:	break;
    }
    out_flush();
    screen_start();
}

    char_u *
mch_getenv(char_u *lognam)
{
    DESC		d_file_dev, d_lognam  ;
    static char		buffer[LNM$C_NAMLENGTH+1];
    char_u		*cp = NULL;
    unsigned long	attrib;
    int			lengte = 0, dum = 0, idx = 0;
    ITMLST2		itmlst;
    char		*sbuf = NULL;

    vul_desc(&d_lognam, (char *)lognam);
    vul_desc(&d_file_dev, "LNM$FILE_DEV");
    attrib = LNM$M_CASE_BLIND;
    vul_item(&itmlst.index, sizeof(int), LNM$_INDEX, (char *)&idx, &dum);
    vul_item(&itmlst.string, LNM$C_NAMLENGTH, LNM$_STRING, buffer, &lengte);
    itmlst.nul	= 0;
    if (sys$trnlnm(&attrib, &d_file_dev, &d_lognam, NULL,&itmlst) == SS$_NORMAL)
    {
	buffer[lengte] = '\0';
	if (cp = (char_u *)alloc((unsigned)(lengte+1)))
	    strcpy((char *)cp, buffer);
	return(cp);
    }
    else if ((sbuf = getenv((char *)lognam)))
    {
	lengte = strlen(sbuf) + 1;
	cp = (char_u *)malloc((size_t)lengte);
	if (cp)
	    strcpy((char *)cp, sbuf);
	return cp;
    }
    else
	return(NULL);
}

/*
 *	mch_setenv	VMS version of setenv()
 */
    int
mch_setenv(char *var, char *value, int x)
{
    int		res, dum;
    long	attrib = 0L;
    char	acmode = PSL$C_SUPER;	/* SYSNAM privilege nodig	*/
    DESC	tabnam, lognam;
    ITMLST1	itmlst;

    vul_desc(&tabnam, "LNM$JOB");
    vul_desc(&lognam, var);
    vul_item(&itmlst.equ, value ? strlen(value) : 0, value ? LNM$_STRING : 0,
	    value, &dum);
    itmlst.nul	= 0;
    res = sys$crelnm(&attrib, &tabnam, &lognam, &acmode, &itmlst);
    return((res == 1) ? 0 : -1);
}

    int
vms_sys(char *cmd, char *out, char *inp)
{
    DESC	cdsc, odsc, idsc;
    long	status, substatus;

    if (cmd)
	vul_desc(&cdsc, cmd);
    if (out)
	vul_desc(&odsc, out);
    if (inp)
	vul_desc(&idsc, inp);

    status = lib$spawn(
	    cmd ? &cdsc : NULL,		/* command string */
	    inp ? &idsc : NULL,		/* input file */
	    out ? &odsc : NULL,		/* output file */
	    0, 0, 0, &substatus, 0, 0, 0, 0, 0, 0);
    if (status != SS$_NORMAL)
	substatus = status;
    if ((substatus&STS$M_SUCCESS) == 0)     /* Command failed.	    */
	return(FALSE);
    return(TRUE);
}

/*
 * vms_wproc() is called for each matching filename by decc$to_vms().
 * We want to save each match for later retrieval.
 *
 * Returns:  1 - continue finding matches
 *	     0 - stop trying to find any further mathces
 *
 */
static int
vms_wproc( char *name )
{
    char xname[MAXPATHL];
    int i;

    if (vms_match_num == 0) {
	/* first time through, setup some things */
	if (NULL == vms_fmatch) {
	    vms_fmatch = (char_u **)alloc(EXPL_ALLOC_INC * sizeof(char *));
	    if (!vms_fmatch)
		return 0;
	    vms_match_alloced = EXPL_ALLOC_INC;
	    vms_match_free = EXPL_ALLOC_INC;
	}
	else {
	    /* re-use existing space */
	    vms_match_free = vms_match_alloced;
	}
    }
    
    strcpy(xname,vms_fixfilename(name));
    vms_remove_version(xname); 

    /* if name already exists, don't add it */
    for (i = 0; i<vms_match_num; i++) {
	if (0 == STRCMP((char_u *)xname,vms_fmatch[i]))
	    return 1;
    }
    if (--vms_match_free == 0) {
	/* add more space to store matches */
	vms_match_alloced += EXPL_ALLOC_INC;
	vms_fmatch = (char_u **)realloc(vms_fmatch,
		sizeof(char **) * vms_match_alloced);
	if (!vms_fmatch)
	    return 0;
	vms_match_free = EXPL_ALLOC_INC;
    }
#ifdef APPEND_DIR_SLASH
    if (type == DECC$K_DIRECTORY) {
	STRCAT(xname,"/");
	vms_fmatch[vms_match_num] = vim_strsave((char_u *)xname);
    }
    else {
	vms_fmatch[vms_match_num] =
	    vim_strsave((char_u *)xname);
    }
#else
    vms_fmatch[vms_match_num] =
	vim_strsave((char_u *)xname);
#endif
    ++vms_match_num;
    return 1;
}

/*
 *	mch_expand_wildcards	this code does wild-card pattern
 *				matching NOT using the shell
 *
 *	return OK for success, FAIL for error (you may loose some
 *	memory) and put an error message in *file.
 *
 *	num_pat	   number of input patterns
 *	pat	   array of pointers to input patterns
 *	num_file   pointer to number of matched file names
 *	file	   pointer to array of pointers to matched file names
 *
 */
    int
mch_expand_wildcards(int num_pat, char_u **pat, int *num_file, char_u ***file, int flags)
{
    int		i, j = 0, cnt = 0;
    char	*cp;
    char_u	buf[MAXPATHL];
    int		dir;
    int files_alloced, files_free;

    *num_file = 0;			/* default: no files found	*/
    files_alloced = EXPL_ALLOC_INC;
    files_free = EXPL_ALLOC_INC;
    *file = (char_u **) alloc(sizeof(char_u **) * files_alloced);
    if (*file == NULL)
    {
	*num_file = 0;
	return FAIL;
    }
    for (i = 0; i < num_pat; i++)
    {
	/* expand environment var or home dir */
	if (vim_strchr(pat[i],'$') || vim_strchr(pat[i],'~'))
	    expand_env(pat[i],buf,MAXPATHL);
	else
	    STRCPY(buf,pat[i]);

	vms_match_num = 0; /* reset collection counter */
	cnt = decc$from_vms( vms_fixfilename(buf), vms_wproc, 1);
	if (cnt > 0)
	    cnt = vms_match_num;

	if (cnt < 1)
	    continue;

	for (i = 0; i < cnt; i++)
	{
	    /* files should exist if expanding interactively */
	    if (!(flags & EW_NOTFOUND) && mch_getperm(vms_fmatch[i]) < 0)
		continue;
	    /* do not include directories */
	    dir = (mch_isdir(vms_fmatch[i]));
	    if (( dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
		continue;
	    /* allocate memory for pointers */
	    if (--files_free < 1)
	    {
		files_alloced += EXPL_ALLOC_INC;
		*file = (char_u **)realloc(*file,
		    sizeof(char_u **) * files_alloced);
		if (*file == NULL)
		{
		    *file = (char_u **)"";
		    *num_file = 0;
		    return(FAIL);
		}
		files_free = EXPL_ALLOC_INC;
	    }

	    (*file)[*num_file++] = vms_fmatch[i];
	}
    }
    return OK;
}

    int
mch_expandpath(garray_T *gap, char_u *path, int flags)
{
    int		i,cnt = 0;
    char	*cp;
    vms_match_num = 0;
    
    cnt = decc$from_vms(vms_fixfilename(path), vms_wproc, 1 );
    if (cnt > 0)
	cnt = vms_match_num;
    for (i = 0; i < cnt; i++)
    {
	if (mch_getperm(vms_fmatch[i]) >= 0) /* add existing file */
	    addfile(gap, vms_fmatch[i], flags);
    }
    return cnt;
}

/* 
 * attempt to translate a mixed unix-vms file specification to pure vms 
 */
    static void
vms_unix_mixed_filespec(char *in, char *out)
{
    char *lastcolon;
    char *end_of_dir;
    char ch;
    int len;

    /* copy vms filename portion up to last colon
     * (node and/or disk)
     */
    lastcolon = strrchr(in, ':');   /* find last colon */
    if (lastcolon != NULL) {
	len = lastcolon - in + 1;
	strncpy(out, in, len);
	out += len;
	in += len;
    }

    end_of_dir = NULL;	/* default: no directory */

    /* start of directory portion */
    ch = *in;
    if ((ch == '[') || (ch == '/') || (ch == '<') ) {	/* start of directory(s) ? */
	ch = '[';
	SKIP_FOLLOWING_SLASHES(in);
    } else if (EQN(in, "../", 3)) { /* Unix parent directory? */
	*out++ = '[';
	*out++ = '-';
	end_of_dir = out;
	ch = '.';
	in += 2;
	SKIP_FOLLOWING_SLASHES(in);
    } else {		    /* not a special character */
	while (EQN(in, "./", 2)) {	/* Ignore Unix "current dir" */
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
    }
    if (strchr(in, '/') == NULL) {  /* any more Unix directories ? */
	strcpy(out, in);	/* No - get rest of the spec */
	return;
    } else {
	*out++ = '[';	    /* Yes, denote a Vms subdirectory */
	ch = '.';
	--in;
	}
    }

    /* if we get here, there is a directory part of the filename */

    /* initialize output file spec */
    *out++ = ch;
    ++in;

    while (*in != '\0') {
	ch = *in;
	if ((ch == ']') || (ch == '/') || (ch == '>') ) {	/* end of (sub)directory ? */
	    end_of_dir = out;
	    ch = '.';
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	else if (EQN(in, "../", 3)) {	/* Unix parent directory? */
	    *out++ = '-';
	    end_of_dir = out;
	    ch = '.';
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    }
	else {
	    while (EQN(in, "./", 2)) {  /* Ignore Unix "current dir" */
	    end_of_dir = out;
	    in += 2;
	    SKIP_FOLLOWING_SLASHES(in);
	    ch = *in;
	    }
	}

    /* Place next character into output file spec */
	*out++ = ch;
	++in;
    }

    *out = '\0';    /* Terminate output file spec */

    if (end_of_dir != NULL) /* Terminate directory portion */
	*end_of_dir = ']';
}


/* 
 * for decc$to_vms in vms_fixfilename 
 */
    static int
vms_fspec_proc(char *fil, int val)
{
    strcpy(Fspec_Rms,fil);
    return(1);
}

/*
 * change '/' to '.' (or ']' for the last one)
 */
    void *
vms_fixfilename(void *instring)
{
    static char		*buf = NULL;
    static size_t	buflen = 0;
    size_t		len;

    /* get a big-enough buffer */
    len = strlen(instring) + 1;
    if (len > buflen)
    {
	buflen = len + 128;
	if (buf)
	    buf = (char *)realloc(buf, buflen);
	else
	    buf = (char *)calloc(buflen, sizeof(char));
    }

#ifdef DEBUG
     char                *tmpbuf = NULL;
     tmpbuf = (char *)calloc(buflen, sizeof(char));
     strcpy(tmpbuf, instring);
#endif

    /* if this already is a vms file specification, copy it
     * else if VMS understands how to translate the file spec, let it do so
     * else translate mixed unix-vms file specs to pure vms
     */

    Fspec_Rms = buf;				/* for decc$to_vms */
    if (strchr(instring,'/') == NULL)
	strcpy(buf, instring);			/* already a vms file spec */
    else if (strchr(instring,'"') == NULL){	/* regular file */
	if (decc$to_vms(instring, vms_fspec_proc, 0, 0) <= 0)
	    vms_unix_mixed_filespec(instring, buf);
    }
    else
	vms_unix_mixed_filespec(instring, buf); /* we have a passwd in the path */

    return buf;
}
/*
 * Remove version number from file name
 * we need it in some special cases as:
 * creating swap file name and writing new file
 */
    void *
vms_remove_version(void * fname)
{
    char_u	*cp;

    if ((cp = vim_strchr( fname, ';')) != NULL) /* remove version */
	*cp = '\0';
    vms_fixfilename(fname);
    return fname;
}
