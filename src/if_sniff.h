/*
 * if_sniff.h Interface between Vim and SNiFF+
 *
 * $Id: if_sniff.h,v 1.4 1998/01/26 14:46:49 toni Exp $
 */

#ifndef __if_sniff_h__
#define __if_sniff_h__

extern int  want_sniff_request;
extern int  sniff_request_waiting;
extern int  sniff_connected;
extern int  fd_from_sniff;
extern void ProcessSniffRequests();
extern void do_sniff __ARGS((char_u *arg));

#endif
