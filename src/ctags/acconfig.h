/* $Id: acconfig.h,v 6.2 1998/07/02 06:10:55 darren Exp $   */

/*  Package name.
 */
#undef PACKAGE

/*  Package version.
 */
#undef VERSION

/*  Define each of these to the appropriate type if no typedef exists.
 */
#undef clock_t

/*  Define to the appropriate size for tmpnam() if <stdio.h> does not define
 *  this.
 */
#undef L_tmpnam

/*  Define remove to unlink if you have unlink(), but not remove().
 */
#undef remove

/*  Define these values used by fseek() appropriately if <stdio.h> does not
 *  define them.
 */
#undef SEEK_SET
#undef SEEK_END

/*  Define as the maximum integer on your system if not defined <limits.h>.
 */
#undef INT_MAX

/*  Define this label if you want macro tags (defined lables) to use patterns
 *  in the EX command by default (original ctags behavior is to use line
 *  numbers).
 */
#undef MACROS_USE_PATTERNS

/*  Define this as desired.
 *    1:  Original ctags format
 *    2:  Extended ctags format with extension flags in EX-style comment.
 */
#define DEFAULT_FILE_FORMAT	2

/*  Define this label to use the system sort utility (which is probably more
 *  efficient) over the interal sorting algorithm.
 */
#undef EXTERNAL_SORT

/*  If you are using the system sort utility (i.e. external sort), define
 *  this label to be the name of your awk program, which is used to report
 *  duplicate tags when the -W option is specified.
 */
#define AWK "awk"

/*  This corrects the problem of missing prototypes for certain functions
 *  in some GNU installations (e.g. SunOS 4.1.x).
 */
#undef __USE_FIXED_PROTOTYPES__

/*  Define this is you have a prototype for putenv() in <stdlib.h>, but
 *  doesn't declare its argument as "const char *".
 */
#undef NON_CONST_PUTENV_PROTOTYPE

/*  If you receive error or warning messages indicating that you are missing
 *  a prototype for, or a type mismatch using, one of the following functions,
 *  define the appropriate label and remake.
 */
#undef NEED_PROTO_REMOVE
#undef NEED_PROTO_UNLINK
#undef NEED_PROTO_MALLOC
#undef NEED_PROTO_GETENV
#undef NEED_PROTO_STRSTR
#undef NEED_PROTO_STAT
#undef NEED_PROTO_TRUNCATE
#undef NEED_PROTO_FTRUNCATE

/*----------------------------------------------------------------------------
-	Lines below this are automatically generated by autoheader
----------------------------------------------------------------------------*/
/* @TOP@ */
