#ifndef	PROTO_ARP_H
#define	PROTO_ARP_H	1

/*
 ************************************************************************
 *	The arp copies of the dos.library calls...			*
 ************************************************************************
 */

/* Only include these if you can use ARP.library without dos.library... */
#ifdef	DO_ARP_COPIES
#pragma	amicall(ArpBase, 0x1E, Open(d1, d2))
#pragma	amicall(ArpBase, 0x24, Close(d1))
#pragma	amicall(ArpBase, 0x2A, Read(d1, d2, d3))
#pragma	amicall(ArpBase, 0x30, Write(d1, d2, d3))
#pragma	amicall(ArpBase, 0x36, Input())
#pragma	amicall(ArpBase, 0x3C, Output())
#pragma	amicall(ArpBase, 0x42, Seek(d1, d2, d3))
#pragma	amicall(ArpBase, 0x48, DeleteFile(d1))
#pragma	amicall(ArpBase, 0x4E, Rename(d1, d2))
#pragma	amicall(ArpBase, 0x54, Lock(d1, d2))
#pragma	amicall(ArpBase, 0x5A, UnLock(d1))
#pragma	amicall(ArpBase, 0x60, DupLock(d1))
#pragma	amicall(ArpBase, 0x66, Examine(d1, d2))
#pragma	amicall(ArpBase, 0x6C, ExNext(d1, d2))
#pragma	amicall(ArpBase, 0x72, Info(d1, d2))
#pragma	amicall(ArpBase, 0x78, CreateDir(d1))
#pragma	amicall(ArpBase, 0x7E, CurrentDir(d1))
#pragma	amicall(ArpBase, 0x84, IoErr())
#pragma	amicall(ArpBase, 0x8A, CreateProc(d1, d2, d3, d4))
#pragma	amicall(ArpBase, 0x90, Exit(d1))
#pragma	amicall(ArpBase, 0x96, LoadSeg(d1))
#pragma	amicall(ArpBase, 0x9C, UnLoadSeg(d1))
#pragma	amicall(ArpBase, 0xAE, DeviceProc(d1))
#pragma	amicall(ArpBase, 0xB4, SetComment(d1, d2))
#pragma	amicall(ArpBase, 0xBA, SetProtection(d1, d2))
#pragma	amicall(ArpBase, 0xC0, DateStamp(d1))
#pragma	amicall(ArpBase, 0xC6, Delay(d1))
#pragma	amicall(ArpBase, 0xCC, WaitForChar(d1, d2))
#pragma	amicall(ArpBase, 0xD2, ParentDir(d1))
#pragma	amicall(ArpBase, 0xD8, IsInteractive(d1))
#pragma	amicall(ArpBase, 0xDE, Execute(d1, d2, d3))
#endif

/*
 ************************************************************************
 *	Stuff only in arp.library					*
 ************************************************************************
 */
/*	amicall(ArpBase, 0x0E4, Printf(a0, a1))	This does not work without glue */
/*	amicall(ArpBase, 0x0EA, FPrintf(d0, a0, a1))	This does not work without glue */
#pragma	amicall(ArpBase, 0x0F0, Puts(a1))
#pragma	amicall(ArpBase, 0x0F6, Readline(a0))
#pragma	amicall(ArpBase, 0x0FC, GADS(a0, d0, a1, a2, a3))
#pragma	amicall(ArpBase, 0x102, Atol(a0))
#pragma	amicall(ArpBase, 0x108, EscapeString(a0))
#pragma	amicall(ArpBase, 0x10E, CheckAbort(a1))
#pragma	amicall(ArpBase, 0x114, CheckBreak(d1, a1))
#pragma	amicall(ArpBase, 0x11A, Getenv(a0, a1, d0))
#pragma	amicall(ArpBase, 0x120, Setenv(a0, a1))
#pragma	amicall(ArpBase, 0x126, FileRequest(a0))
#pragma	amicall(ArpBase, 0x12C, CloseWindowSafely(a0, a1))
#pragma	amicall(ArpBase, 0x132, CreatePort(a0, d0))
#pragma	amicall(ArpBase, 0x138, DeletePort(a1))
#pragma	amicall(ArpBase, 0x13E, SendPacket(d0, a0, a1))
#pragma	amicall(ArpBase, 0x144, InitStdPacket(d0, a0, a1, a2))
#pragma	amicall(ArpBase, 0x14A, PathName(d0, a0, d1))
#pragma	amicall(ArpBase, 0x150, Assign(a0, a1))
#pragma	amicall(ArpBase, 0x156, DosAllocMem(d0))
#pragma	amicall(ArpBase, 0x15C, DosFreeMem(a1))
#pragma	amicall(ArpBase, 0x162, BtoCStr(a0, d0, d1))
#pragma	amicall(ArpBase, 0x168, CtoBStr(a0, d0, d1))
#pragma	amicall(ArpBase, 0x16E, GetDevInfo(a2))
#pragma	amicall(ArpBase, 0x174, FreeTaskResList())
#pragma	amicall(ArpBase, 0x17A, ArpExit(d0, d2))
#pragma	amicall(ArpBase, 0x180, ArpAlloc(d0))
/*	amicall(ArpBase, 0x186, ArpAllocMem(d0, d1))	Secondary result - IoErr() */
/*	amicall(ArpBase, 0x18C, ArpOpen(d1, d2))	Secondary result - IoErr() */
/*	amicall(ArpBase, 0x192, ArpDupLock(d1))		Secondary result - IoErr() */
/*	amicall(ArpBase, 0x198, ArpLock(d1, d2))	Secondary result - IoErr() */
/*	amicall(ArpBase, 0x19E, RListAlloc(a0, d0))	Secondary result - IoErr() */
#pragma	amicall(ArpBase, 0x1A4, FindCLI(d0))
#pragma	amicall(ArpBase, 0x1AA, QSort(a0, d0, d1, a1))

#pragma	amicall(ArpBase, 0x1B0, PatternMatch(a0, a1))
#pragma	amicall(ArpBase, 0x1B6, FindFirst(d0, a0))
#pragma	amicall(ArpBase, 0x1BC, FindNext(a0))
#pragma	amicall(ArpBase, 0x1C2, FreeAnchorChain(a0))

#pragma	amicall(ArpBase, 0x1C8, CompareLock(d0, d1))

#pragma	amicall(ArpBase, 0x1CE, FindTaskResList())
#pragma	amicall(ArpBase, 0x1D4, CreateTaskResList())
#pragma	amicall(ArpBase, 0x1DA, FreeResList())
#pragma	amicall(ArpBase, 0x1E0, FreeTrackedItem(a1))
/*	amicall(ArpBase, 0x1E6, GetTracker())	Stores the ID in the tracker */

#pragma	amicall(ArpBase, 0x1EC, GetAccess(a1))
#pragma	amicall(ArpBase, 0x1F2, FreeAccess(a1))

#pragma	amicall(ArpBase, 0x1F8, FreeDAList(a1))
#pragma	amicall(ArpBase, 0x1FE, AddDANode(a0, a1, d0, d1))
#pragma	amicall(ArpBase, 0x204, AddDADevs(a0, d0))

#pragma	amicall(ArpBase, 0x20A, Strcmp(a0, a1))
#pragma	amicall(ArpBase, 0x210, Strncmp(a0, a1, d0))
#pragma	amicall(ArpBase, 0x216, Toupper(d0))
#pragma	amicall(ArpBase, 0x21C, SyncRun(a0, a1, d0, d1))

/*
 ************************************************************************
 *	Added V32 of arp.library					*
 *	Note that SpawnShell is ASyncRun but was added at V39 of arp...	*
 ************************************************************************
 */
#pragma	amicall(ArpBase, 0x222, ASyncRun(a0, a1, a2))
#pragma	amicall(ArpBase, 0x222, SpawnShell(a0, a1, a2))
#pragma	amicall(ArpBase, 0x228, LoadPrg(d1))
#pragma	amicall(ArpBase, 0x22E, PreParse(a0, a1))

/*
 ************************************************************************
 *	Added V33 of arp.library					*
 ************************************************************************
 */
#pragma	amicall(ArpBase, 0x234, StamptoStr(a0))
#pragma	amicall(ArpBase, 0x23A, StrtoStamp(a0))

#pragma	amicall(ArpBase, 0x240, ObtainResidentPrg(a0))
#pragma	amicall(ArpBase, 0x246, AddResidentPrg(d1, a0))
#pragma	amicall(ArpBase, 0x24C, RemResidentPrg(a0))
#pragma	amicall(ArpBase, 0x252, UnLoadPrg(d1))
#pragma	amicall(ArpBase, 0x258, LMult(d0, d1))
#pragma	amicall(ArpBase, 0x25E, LDiv(d0, d1))
#pragma	amicall(ArpBase, 0x264, LMod(d0, d1))

#pragma	amicall(ArpBase, 0x26A, CheckSumPrg(d1))
#pragma	amicall(ArpBase, 0x270, TackOn(a0, a1))
#pragma	amicall(ArpBase, 0x276, BaseName(a0))
#pragma	amicall(ArpBase, 0x27C, ReleaseResidentPrg(d1))

/*
 ************************************************************************
 *	Added V36 of arp.library					*
 ************************************************************************
 */
/*	amicall(ArpBase, 0x282, SPrintf(d0, a0, a1))	This does not work without glue */
#pragma	amicall(ArpBase, 0x288, GetKeywordIndex(a0, a1))
/*	amicall(ArpBase, 0x28E, ArpOpenLibrary(a1, d0))	Secondary result - IoErr() */
#pragma	amicall(ArpBase, 0x294, ArpAllocFreq())

#endif
