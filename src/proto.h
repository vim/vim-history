/* vi:ts=4:sw=4
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Read the file "credits.txt" for a list of people who contributed.
 * Read the file "uganda.txt" for copying and usage conditions.
 */

/*
 * proto.h: include the (automatically generated) function prototypes
 *
 * the proto/xxx.pro files are automatically generated when using Manx/Aztec C.
 * For other compilers you will have to edit them.
 */

#include "regexp.h"		/* for struct regexp */

/*
 * Machine-dependent routines.
 */
#ifdef AMIGA
# include "proto/amiga.pro"
#endif
#ifdef UNIX
# include "proto/unix.pro"
#endif
#ifdef MSDOS
# include "proto/msdos.pro"
#endif

#include "proto/alloc.pro"
#include "proto/buffer.pro"
#include "proto/charset.pro"
#include "proto/cmdcmds.pro"
#include "proto/cmdline.pro"
#include "proto/csearch.pro"
#include "proto/digraph.pro"
#include "proto/edit.pro"
#include "proto/fileio.pro"
#include "proto/getchar.pro"
#include "proto/help.pro"
#include "proto/linefunc.pro"
#include "proto/main.pro"
#include "proto/mark.pro"

#ifndef MESSAGE
void smsg __PARMS((char_u *, ...));	/* cannot be produced automatically */
#endif
#include "proto/memfile.pro"
#include "proto/memline.pro"
#include "proto/message.pro"
#include "proto/misccmds.pro"
#include "proto/normal.pro"
#include "proto/ops.pro"
#include "proto/param.pro"
#include "proto/quickfix.pro"
#include "proto/regexp.pro"
#include "proto/regsub.pro"
#include "proto/screen.pro"
#include "proto/search.pro"
#include "proto/tag.pro"
#include "proto/term.pro"
#include "proto/undo.pro"
#include "proto/window.pro"
