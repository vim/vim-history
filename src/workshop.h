/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *			Visual Workshop integration by Gordon Prieur
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ifndef WORKSHOP_H
#define WORKSHOP_H

#ifdef FEAT_SUN_WORKSHOP

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include "integration.h" /* was "EditPlugin/integration.h" */

#ifdef WSDEBUG
# include "wsdebug.h"
#endif

extern int	usingSunWorkShop;	/* set if -ws flag is used */


#ifdef WSDEBUG

#ifndef ASSERT
#define ASSERT(c) \
    if (!(c)) { \
        fprintf(stderr, "Assertion failed: line %d, file %s\n", \
                __LINE__, __FILE__); \
        fflush(stderr); \
        abort(); \
    }
#endif

#define WS_TRACE		0x00000001
#define WS_TRACE_VERBOSE	0x00000002
#define WS_DEBUG_ALL		0xffffffff

#define WSDLEVEL(flags)		(debug != NULL && (dlevel & (flags)))

#ifdef USE_WDDUMP
#include "wdump.h"
#endif

#else

#define ASSERT(c)

#endif /* WSDEBUG */


#endif /* FEAT_SUN_WORKSHOP */
#endif /* WORKSHOP_H */
