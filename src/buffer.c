/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * buffer.c: functions for dealing with the buffer structure
 */

#include "vim.h"
#include "globals.h"
#include "proto.h"
#include "param.h"

/*
 * open a buffer to start editing
 *
 * If a buffer for the same file already exists, use that one.
 * If fname is NULL, create an empty buffer.
 * Close the current buffer and may free it if "free_buf" is TRUE.
 *
 * return FAIL for failure, OK for success
 */
	int
open_buffer(fname, oldbuf, free_buf)
	char_u		*fname;
	int			*oldbuf;
	int			free_buf;
{
	BUF		*buf;

	if (fname == NULL)
		buf = NULL;
	else
		for (buf = firstbuf; buf; buf = buf->b_next)
			if (buf->b_filename != NULL && fnamecmp(fname, buf->b_filename) == 0)
				break;
	if (buf == NULL)			/* need to allocate a new buffer */
	{
		buf = buf_alloc();
		if (buf == NULL)
			return FAIL;
		buf_init(buf);
		buf_copy_options(curbuf, buf);
		buf->b_nwindows = 1;
		*oldbuf = FALSE;
	}
	else
	{
		++buf->b_nwindows;
		*oldbuf = TRUE;
	}
/*
 * make the (new) buffer the one used by the current window
 * if the old buffer becomes unused, free it
 */
	close_buffer(curbuf, free_buf);
	curwin->w_buffer = buf;
	curbuf = buf;

	return OK;
}

/*
 * allocate an empty buffer
 */
	BUF *
buf_alloc()
{
	BUF 	*buf;
	BUF		*last;

	buf = (BUF *)alloc((unsigned)sizeof(BUF));
	if (buf != NULL)
	{
		memset((char *)buf, 0, sizeof(BUF));
		buf->b_u_synced = TRUE;

		buf->b_next = NULL;
		if (firstbuf == NULL)		/* buffer list is empty */
		{
			buf->b_prev = NULL;
			firstbuf = buf;
		}
		else						/* append new buffer at end of the list */
		{
			for (last = firstbuf; last->b_next != NULL; last = last->b_next)
				;
			last->b_next = buf;
			buf->b_prev = last;
		}
	}
	return buf;
}

/*
 * Close the link to a buffer. If "free_buf" is TRUE free the buffer if it
 * becomes unreferenced. The caller should get a new buffer very soon!
 */
	void
close_buffer(buf, free_buf)
	BUF		*buf;
	int		free_buf;
{
	if (buf->b_nwindows > 0)
		--buf->b_nwindows;
	if (buf->b_nwindows > 0 || !free_buf)
	{
		if (buf == curbuf)
			u_sync();		/* sync undo before going to another buffer */
		return;
	}

	buf_freeall(buf);		/* free all things allocated for this buffer */

	if (buf->b_next)
		buf->b_next->b_prev = buf->b_prev;
	if (buf->b_prev)
		buf->b_prev->b_next = buf->b_next;
	else
		firstbuf = buf->b_next;
	free(buf);
}

/*
 * buf_init() - make buffer empty
 */
	void
buf_init(buf)
	BUF		*buf;
{
	buf->b_ml.ml_line_count = 1;
	buf->b_changed = FALSE;
#ifndef MSDOS
	buf->b_shortname = FALSE;
#endif
	buf->b_p_eol = TRUE;
	buf->b_ml.ml_mfp = NULL;
	buf->b_ml.ml_flags = ML_EMPTY;					/* empty buffer */
}

/*
 * buf_freeall() - free all things allocated for the buffer
 */
	void
buf_freeall(buf)
	BUF		*buf;
{
	u_blockfree(buf);				/* free the memory allocated for undo */
	ml_close(buf);					/* close and delete the memline/memfile */
	buf->b_ml.ml_line_count = 0;	/* no lines in buffer */
	u_clearall(buf);				/* reset all undo information */
	clrallmarks(buf);				/* clear marks */
}

/*
 * do_buffers() - show a list of all existing buffers
 */
	void
do_buffers()
{
	BUF		*buf;
	int		n;

	msg_start();
	mch_start_listing();
	for (n = 1, buf = firstbuf; buf != NULL; buf = buf->b_next, ++n)
	{
		if (buf != firstbuf)
			msg_outchar('\n');
		if (n < 10)
			msg_outchar(' ');
		msg_outnum((long)n);
		if (buf == curbuf)					/* current buffer */
			msg_outchar('>');
		else if (buf->b_nwindows == 0)		/* hidden buffer */
			msg_outchar('h');
		else
			msg_outchar(' ');
		if (buf->b_changed)
			msg_outchar('*');
		else
			msg_outchar(' ');
		msg_outchar(' ');
		if (buf->b_xfilename == NULL)
			msg_outstr((char_u *)"No_File");
		else
			msg_outstr(buf->b_xfilename);
	}
	mch_stop_listing();
	msg_end();
}
