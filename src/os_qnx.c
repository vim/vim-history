/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * QNX port by Julian Kinraid
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_qnx.c
 */

#include "vim.h"


int is_photon_available;


void qnx_init()
{
#if defined(FEAT_CLIPBOARD) || defined(FEAT_GUI)
    is_photon_available = (PhAttach( NULL, NULL ) != NULL) ? TRUE : FALSE;
#endif
}

#if defined(FEAT_CLIPBOARD) || defined(PROTO)

#define CLIP_TYPE_VIM "TEXTVIM"
#define CLIP_TYPE_TEXT "TEXT"

void qnx_clip_init()
{
    if( is_photon_available == TRUE )
    {
	clip_init( TRUE );
    }
}

/*****************************************************************************/
/* Clipboard */

/* No support for owning the clipboard */
int
clip_mch_own_selection( VimClipboard *cbd )
{
    return FALSE;
}

void
clip_mch_lose_selection( VimClipboard *cbd )
{
}

void
clip_mch_request_selection( VimClipboard *cbd )
{
    int		    type = MLINE, clip_length = 0;
    void	    *cbdata;
    PhClipHeader    *clip_header;
    char_u	    *clip_text = NULL;

    cbdata = PhClipboardPasteStart( PhInputGroup( NULL ));
    if( cbdata != NULL )
    {
	/* Look for the vim specific clip first */
	clip_header = PhClipboardPasteType( cbdata, CLIP_TYPE_VIM );
	if( clip_header != NULL )
	{
	    clip_text = clip_header->data;
	    /* Skip past the initial type specifier */
	    /* clip_header->length also includes the trailing NUL */
	    clip_length  = clip_header->length - 2;

	    if( clip_text != NULL )
	    {
		switch( *clip_text++ )
		{
		    default: /* fallthrough to line type */
		    case 'L': type = MLINE; break;
		    case 'C': type = MCHAR; break;
		    case 'B': type = MBLOCK; break;
		}
	    }
	}
	else
	{
	    /* Try for just normal text */
	    clip_header = PhClipboardPasteType( cbdata, CLIP_TYPE_TEXT );
	    if( clip_header != NULL )
	    {
		clip_text = clip_header->data;
		clip_length  = clip_header->length - 1;

		if( clip_text != NULL )
		    type = (strchr( clip_text, '\r' ) != NULL) ? MLINE : MCHAR;
	    }
	}

	if( (clip_text != NULL) && (clip_length > 0) )
	{
	    clip_yank_selection( type, clip_text, clip_length, cbd );
	}

	PhClipboardPasteFinish( cbdata );
    }
}

void
clip_mch_set_selection( VimClipboard *cbd )
{
    int type;
    long_u  len;
    char_u *text_clip, *vim_clip, *str = NULL;

    cbd->owned = TRUE;
    clip_get_selection( cbd );
    cbd->owned = FALSE;

    type = clip_convert_selection( &str, &len, cbd );
    if( type >= 0 )
    {
	text_clip = lalloc( len + 1, TRUE ); /* Normal text */
	vim_clip  = lalloc( len + 2, TRUE ); /* vim specific info + text */

	if( text_clip && vim_clip )
	{
	    PhClipHeader clip_header[2];

	    STRNCPY( clip_header[0].type, CLIP_TYPE_VIM, 8 );
	    clip_header[0].length = len + 2;
	    clip_header[0].data   = vim_clip;

	    STRNCPY( clip_header[1].type, CLIP_TYPE_TEXT, 8 );
	    clip_header[1].length = len + 1;
	    clip_header[1].data   = text_clip;

	    switch( type )
	    {
		default: /* fallthrough to MLINE */
		case MLINE:	*vim_clip = 'L'; break;
		case MBLOCK:	*vim_clip = 'B'; break;
		case MCHAR:	*vim_clip = 'C'; break;
	    }

	    STRNCPY( text_clip, str, len );
	    text_clip[ len ] = NUL;

	    STRNCPY( vim_clip + 1, str, len );
	    vim_clip[ len + 1 ] = NUL;

	    PhClipboardCopy( PhInputGroup( NULL ), 2, clip_header);
	}
	vim_free( text_clip );
	vim_free( vim_clip );
    }
    vim_free( str );
}
#endif
