/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.03.0110 */
/* at Mon Feb 01 19:13:42 1999
 */
/* Compiler settings for D:\Src2\VisVim\VisVim.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID LIBID_VisVim = {0xAC726707,0x2977,0x11D1,{0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80}};


const IID IID_ICommands = {0xAC726703,0x2977,0x11D1,{0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80}};


const CLSID CLSID_Commands = {0xAC726704,0x2977,0x11D1,{0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80}};


const CLSID CLSID_ApplicationEvents = {0xAC726705,0x2977,0x11D1,{0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80}};


const CLSID CLSID_DebuggerEvents = {0xAC726706,0x2977,0x11D1,{0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80}};


#ifdef __cplusplus
}
#endif

