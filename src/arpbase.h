#ifndef	LIBRARIES_ARPBASE_H
#define	LIBRARIES_ARPBASE_H 1

/*
 ************************************************************************
 *									*
 * 5/3/89	ARPbase.h	by MKSoft from ARPbase.i by SDB		*
 *									*
 ************************************************************************
 *									*
 *	AmigaDOS Resource Project -- Library Include File		*
 *				     for Lattice C 5.x or Manx C 5.x	*
 *									*
 ************************************************************************
 *									*
 *	Copyright (c) 1987/1988/1989 by Scott Ballantyne		*
 *									*
 *	The arp.library, and related code and files may be freely used	*
 *	by supporters of ARP.  Modules in the arp.library may not be	*
 *	extracted for use in independent code, but you are welcome to	*
 *	provide the arp.library with your work and call on it freely.	*
 *									*
 *	You are equally welcome to add new functions, improve the ones	*
 *	within, or suggest additions.					*
 *									*
 *	BCPL programs are not welcome to call on the arp.library.	*
 *	The welcome mat is out to all others.				*
 *									*
 ************************************************************************
 *									*
 * N O T E !  You MUST! have IoErr() defined as LONG to use LastTracker *
 *	      If your compiler has other defines for this, you may wish *
 *	      to remove the prototype for IoErr() from this file.	*
 *									*
 ************************************************************************
 */

/*
 ************************************************************************
 *	First we need to include the Amiga Standard Include files...	*
 ************************************************************************
 */
#ifndef	EXEC_TYPES_H
#include <exec/types.h>
#endif	/* EXEC_TYPES_H */

#ifndef	EXEC_LISTS_H
#include <exec/lists.h>
#endif	/* EXEC_LISTS_H */

#ifndef	EXEC_ALERTS_H
#include <exec/alerts.h>
#endif	/* EXEC_ALERTS_H */

#ifndef	EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif	/* EXEC_LIBRARIES_H */

#ifndef	EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif	/* EXEC_SEMAPHORES_H */

#ifndef	LIBRARIES_DOS_H
#include <libraries/dosextens.h>
#endif	/* LIBRARIES_DOS_H */

/*
 ************************************************************************
 *	Check for MANX/Lattice and define the differences...		*
 ************************************************************************
 *	At the moment MANX 3.6 does not have prototypes or the		*
 *	wonderful #pragma statements of Lattice 5.0...			*
 *	And, no __stdargs in MANX either...				*
 ************************************************************************
 */
#ifdef	AZTEC_C

	/* Do we have an old 3.6a compiler? -olsen */

#ifndef __VERSION
#define __VERSION 360
#endif	/* __VERSION */

	/* If this is an old compiler, don't confuse it with
	 * ANSI prototypes and pragmas. -olsen
	 */

#if __VERSION < 500
#define	NO_PRAGMAS	1
#define NO_PROTOTYPES	1
#endif	/* __VERSION */

#define	C_Args

#endif	/* AZTEC_C */

#ifdef	LATTICE

#define	C_Args	__stdargs

#endif	/* LATTICE */

/*
 ************************************************************************
 *	Standard definitions for arp library information		*
 ************************************************************************
 */
#define	ArpName		"arp.library"	/* Name of library... */
#define	ArpVersion	39L		/* Current version... */

/*
 ************************************************************************
 *	The current ARP library node...					*
 ************************************************************************
 */
struct	ArpBase	{
	struct	Library			LibNode;	 /* Standard library node		*/
		APTR			DosRootNode;	 /* Copy of dl_Root			*/
		UBYTE			Flags;		 /* See bitdefs below			*/
		UBYTE			ESCChar; 	 /* Character to be used for escaping	*/
		LONG			ArpReserved1;	 /* ArpLib's use only!!			*/
	struct	Library			*EnvBase; 	 /* Dummy library for MANX compatibility*/
	struct	Library			*DosBase; 	 /* Cached DosBase			*/
	struct	Library			*GfxBase; 	 /* Cached GfxBase			*/
	struct	Library			*IntuiBase;	 /* Cached IntuitionBase		*/
	struct	MinList			ResLists;	 /* Resource trackers			*/
	struct	ResidentProgramNode	*ResidentPrgList;/* Resident Programs.			*/
	struct	SignalSemaphore		ResPrgProtection;/* protection for above		*/
		BPTR			SegList; 	 /* Pointer to loaded libcode (a BPTR).	*/
		};

/*
 ************************************************************************
 *	The following is here *ONLY* for information and for		*
 *	compatibility with MANX.  DO NOT use in new code!		*
 ************************************************************************
 */
#ifdef	ARP_PRIVATE
struct EnvBase {
	struct	Library	LibNode;	/* Standard library node for linkage	*/
		BYTE	*EnvSpace;	/* Access only when Forbidden!		*/
		ULONG	EnvSize;	/* Total allocated mem for EnvSpace	*/
	struct	ArpBase	*ArpBase;	/* Added in V32 for Resource Tracking	*/
		};
#endif	/* ARP_PRIVATE */

/*
 ************************************************************************
 *	These are used in release 33.4 but not by the library code.	*
 *	Instead, individual programs check for these flags.		*
 ************************************************************************
 */
#define	ARPB_WILD_WORLD 0L		; Mixed BCPL/Normal wildcards.
#define	ARPB_WILD_BCPL  1L		; Pure BCPL wildcards.

#define	ARPF_WILD_WORLD (1L << ARPB_WILD_WORLD)
#define	ARPF_WILD_BCPL  (1L << ARPB_WILD_BCPL)

/*
 ************************************************************************
 * The alert object is what you use if you really must return an alert	*
 * to the user. You would normally OR this with another alert number	*
 * from the alerts.h file. Generally, should be NON deadend alerts.	*
 *									*
 * For example, if you can't open ArpLibrary:				*
 *	Alert( (AG_OpenLib|AO_ArpLib), 0L);				*
 ************************************************************************
 */
#define	AO_ArpLib	0x00008036L		/* Alert object */

/*
 ************************************************************************
 *	Alerts that arp.library may return...				*
 ************************************************************************
 */
#define	AN_ArpLib	0x03600000L	/* Alert number				*/
#define	AN_ArpNoMem	0x03610000L	/* No more memory			*/
#define	AN_ArpInputMem	0x03610002L	/* No memory for input buffer		*/
#define	AN_ArpNoMakeEnv	0x83610003L	/* No memory to make EnvLib		*/

#define	AN_ArpNoDOS	0x83630001L	/* Can't open dos.library		*/
#define	AN_ArpNoGfx	0x83630002L	/* Can't open graphics.library		*/
#define	AN_ArpNoIntuit	0x83630003L	/* Can't open intuition			*/
#define	AN_BadPackBlues	0x83640000L	/* Bad packet returned to SendPacket()	*/
#define	AN_Zombie	0x83600003L	/* Zombie roaming around system		*/

#define	AN_ArpScattered	0x83600002L	/* Scatter loading not allowed for arp	*/


/*
 ************************************************************************
 *	Return codes you can get from calling ARP Assign()...		*
 ************************************************************************
 */
#define	ASSIGN_OK	0L	/* Everything is cool and groovey			*/
#define	ASSIGN_NODEV	1L	/* "Physical" is not valid for assignment		*/
#define	ASSIGN_FATAL	2L	/* Something really icky happened			*/
#define	ASSIGN_CANCEL	3L	/* Tried to cancel something but it won't cancel	*/

/*
 ************************************************************************
 *	Size of buffer you need if you are going to call ReadLine()	*
 ************************************************************************
 */
#define	MaxInputBuf	256L

/*
 ************************************************************************
 *	The ARP file requester data structure...			*
 ************************************************************************
 */

	/* This whole part has to be skipped if libraries/asl.h is
	 * pulled in before arpbase.h is included (which is the recommended
	 * sequence). -olsen
	 */

#ifndef LIBRARIES_ASL_H
#define LIBRARIES_ASL_H 1	/* mool: don't use libraries/asl.h */

	/* You know req.library? -olsen */

#ifndef REQLIBRARY_H	

struct FileRequester	{
			BYTE	*fr_Hail;		/* Hailing text			*/
			BYTE	*fr_File;		/* Filename array (FCHARS + 1)	*/
			BYTE	*fr_Dir;		/* Directory array (DSIZE + 1)	*/
		struct	Window	*fr_Window;		/* Window requesting or NULL	*/
			UBYTE	fr_FuncFlags;		/* Set bitdef's below		*/
			UBYTE	fr_Flags2;		/* New flags...			*/
			VOID	(*fr_Function)();	/* Your function, see bitdef's	*/
			WORD	fr_LeftEdge;		/* To be used later...		*/
			WORD	fr_TopEdge;
			};

#endif	/* REQLIBRARY_H */

/*
 ************************************************************************
 * The following are the defines for fr_FuncFlags.  These bits tell	*
 * FileRequest() what your fr_UserFunc is expecting, and what		*
 * FileRequest() should call it for.					*
 *									*
 * You are called like so:						*
 * fr_Function(Mask, Object)						*
 * ULONG	Mask;							*
 * CPTR		*Object;						*
 *									*
 * The Mask is a copy of the flag value that caused FileRequest() to	*
 * call your function. You can use this to determine what action you	*
 * need to perform, and exactly what Object is, so you know what to do	*
 * and what to return.							*
 ************************************************************************
 */
#define	FRB_DoWildFunc	7L /* Call me with a FIB and a name, ZERO return accepts.	*/
#define	FRB_DoMsgFunc	6L /* You get all IDCMP messages not for FileRequest()		*/
#define	FRB_DoColor	5L /* Set this bit for that new and different look		*/
#define	FRB_NewIDCMP	4L /* Force a new IDCMP (only if fr_Window != NULL)		*/
#define	FRB_NewWindFunc	3L /* You get to modify the newwindow structure.		*/
#define	FRB_AddGadFunc	2L /* You get to add gadgets.					*/
#define	FRB_GEventFunc	1L /* Function to call if one of your gadgets is selected.	*/
#define	FRB_ListFunc	0L /* Not implemented yet.					*/

#define	FRF_DoWildFunc	(1L << FRB_DoWildFunc)
#define	FRF_DoMsgFunc	(1L << FRB_DoMsgFunc)
#define	FRF_DoColor	(1L << FRB_DoColor)
#define	FRF_NewIDCMP	(1L << FRB_NewIDCMP)
#define	FRF_NewWindFunc	(1L << FRB_NewWindFunc)
#define	FRF_AddGadFunc	(1L << FRB_AddGadFunc)
#define	FRF_GEventFunc	(1L << FRB_GEventFunc)
#define	FRF_ListFunc	(1L << FRB_ListFunc)

/*
 ************************************************************************
 * The FR2B_ bits are for fr_Flags2 in the file requester structure	*
 ************************************************************************
 */
#define	FR2B_LongPath	0L /* Specify the fr_Dir buffer is 256 bytes long */

#define	FR2F_LongPath	(1L << FR2B_LongPath)

/*
 ************************************************************************
 *	The sizes of the different buffers...				*
 ************************************************************************
 */
#define	FCHARS		32L	/* Filename size				*/
#define	DSIZE		33L	/* Directory name size if not FR2B_LongPath	*/

#define	LONG_DSIZE	254L	/* If FR2B_LongPath is set, use LONG_DSIZE	*/
#define	LONG_FSIZE	126L	/* For compatibility with ARPbase.i		*/

#define	FR_FIRST_GADGET	0x7680L	/* User gadgetID's must be less than this value	*/

#endif	/* LIBRARIES_ASL_H */

#ifndef DOS_DOSASL_H		/* mool: either this or dos/dosasl.h */
#define DOS_DOSASL_H
/*
 ************************************************************************
 * Structure expected by FindFirst()/FindNext()				*
 *									*
 * You need to allocate this structure and initialize it as follows:	*
 *									*
 * Set ap_BreakBits to the signal bits (CDEF) that you want to take a	*
 * break on, or NULL, if you don't want to convenience the user.	*
 *									*
 * if you want to have the FULL PATH NAME of the files you found,	*
 * allocate a buffer at the END of this structure, and put the size of	*
 * it into ap_StrLen.  If you don't want the full path name, make sure	*
 * you set ap_StrLen to zero.  In this case, the name of the file, and	*
 * stats are available in the ap_Info, as per usual.			*
 *									*
 * Then call FindFirst() and then afterwards, FindNext() with this	*
 * structure.  You should check the return value each time (see below)	*
 * and take the appropriate action, ultimately calling			*
 * FreeAnchorChain() when there are no more files and you are done.	*
 * You can tell when you are done by checking for the normal AmigaDOS	*
 * return code ERROR_NO_MORE_ENTRIES.					*
 *									*
 * You will also have to check the DirEntryType variable in the ap_Info	*
 * structure to determine what exactly you have received.		*
 ************************************************************************
 */
struct	AnchorPath	{
		struct	AChain		*ap_Base;	/* Pointer to first anchor			*/
		struct	AChain		*ap_Last;	/* Pointer to last anchor			*/
			LONG		ap_BreakBits;	/* Bits to break on				*/
			LONG		ap_FoundBreak;	/* Bits we broke on. Also returns ERROR_BREAK	*/
			BYTE		ap_Flags;	/* New use for the extra word...		*/
			BYTE		ap_Reserved;	/* To fill it out...				*/
			WORD		ap_StrLen;	/* This is what used to be ap_Length		*/
		struct	FileInfoBlock	ap_Info;
			BYTE		ap_Buf[1];	/* Allocate a buffer here, if desired		*/
			};

#define ap_Length ap_StrLen

/*
 ************************************************************************
 *	Bit definitions for the new ap_Flags...				*
 ************************************************************************
 */
#define	APB_DoWild	0L	/* User option ALL				*/
#define	APB_ItsWild	1L	/* Set by FindFirst, used by FindNext		*/
#define	APB_DoDir	2L	/* Bit is SET if a DIR node should be entered	*/
				/* Application can RESET this bit to AVOID	*/
				/* entering a dir.				*/
#define	APB_DidDir	3L	/* Bit is set for an "expired" dir node		*/
#define	APB_NoMemErr	4L	/* Set if there was not enough memory		*/
#define	APB_DoDot	5L	/* If set, '.' (DOT) will convert to CurrentDir	*/

#define	APF_DoWild	(1L << APB_DoWild)
#define	APF_ItsWild	(1L << APB_ItsWild)
#define	APF_DoDir	(1L << APB_DoDir)
#define	APF_DidDir	(1L << APB_DidDir)
#define	APF_NoMemErr	(1L << APB_NoMemErr)
#define	APF_DoDot	(1L << APB_DoDot)

/*
 ************************************************************************
 * Structure used by the pattern matching functions, no need to obtain,	*
 * diddle or allocate this yourself.					*
 *									*
 * Note:  If you did, you will now break as it has changed...		*
 ************************************************************************
 */
#ifdef	ARP_PRIVATE
struct	AChain	{
	struct	AChain		*an_Child;
	struct	AChain		*an_Parent;
	struct	FileLock	*an_Lock;
	struct	FileInfoBlock	*an_Info;
		BYTE		an_Flags;
		char		an_String[1];	/* Just as is .i file	*/
		};				/* ???  Don't use this!	*/
#endif	/* ARP_PRIVATE */

#define	DDB_PatternBit	0L
#define	DDB_ExaminedBit	1L
#define	DDB_Completed	2L
#define	DDB_AllBit	3L

#define	DDF_PatternBit	(1L << DDB_PatternBit)
#define	DDF_ExaminedBit	(1L << DDB_ExaminedBit)
#define	DDF_Completed	(1L << DDB_Completed)
#define	DDF_AllBit	(1L << DDB_AllBit)

/*
 ************************************************************************
 * This structure takes a pointer, and returns FALSE if wildcard was	*
 * not found by FindFirst()						*
 ************************************************************************
 */
#define	IsWild( ptr )		( *((LONG *)(ptr)) )

/*
 ************************************************************************
 * Constants used by wildcard routines					*
 *									*
 * These are the pre-parsed tokens referred to by pattern match.  It	*
 * is not necessary for you to do anything about these, FindFirst()	*
 * FindNext() handle all these for you.					*
 ************************************************************************
 */
#define	P_ANY		0x80L	/* Token for '*' | '#?'	*/
#define	P_SINGLE	0x81L	/* Token for '?'	*/

/*
 ************************************************************************
 * No need to muck with these as they may change...			*
 ************************************************************************
 */
#ifdef	ARP_PRIVATE
#define	P_ORSTART	0x82L	/* Token for '('	*/
#define	P_ORNEXT	0x83L	/* Token for '|'	*/
#define	P_OREND		0x84L	/* Token for ')'	*/
#define	P_NOT		0x85L	/* Token for '~'	*/
#define	P_NOTCLASS	0x87L	/* Token for '^'	*/
#define	P_CLASS		0x88L	/* Token for '[]'	*/
#define	P_REPBEG	0x89L	/* Token for '['	*/
#define	P_REPEND	0x8AL	/* Token for ']'	*/
#endif	/* ARP_PRIVATE */

#define	ERROR_BUFFER_OVERFLOW	303L	/* User or internal buffer overflow	*/
#define	ERROR_BREAK		304L	/* A break character was received	*/
#define	ERROR_NOT_EXECUTABLE	305L	/* A file has E bit cleared		*/
	/* dos/dosasl.h uses a good lot of the symbols and structures
	 * defined here (AnchorPatch, AChain, ERROR_BREAK and the
	 * like), so let's don't include it again.
	 */

/* #define DOS_DOSASL_H 1 */
#endif	/* added by mool */

/*
 ************************************************************************
 * Structure used by AddDANode(), AddDADevs(), FreeDAList().		*
 *									*
 * This structure is used to create lists of names, which normally	*
 * are devices, assigns, volumes, files, or directories.		*
 ************************************************************************
 */
struct	DirectoryEntry	{
		struct	DirectoryEntry	*de_Next;	/* Next in list				*/
			BYTE		de_Type;	/* DLX_mumble				*/
			BYTE		de_Flags;	/* For future expansion, DO NOT USE!	*/
			BYTE		de_Name[1];	/* The name of the thing found		*/
			};

/*
 ************************************************************************
 * Defines you use to get a list of the devices you want to look at.	*
 * For example, to get a list of all directories and volumes, do:	*
 *									*
 *	AddDADevs( mydalist, (DLF_DIRS | DLF_VOLUMES) )			*
 *									*
 * After this, you can examine the de_type field of the elements added	*
 * to your list (if any) to discover specifics about the objects added.	*
 *									*
 * Note that if you want only devices which are also disks, you must	*
 * (DLF_DEVICES | DLF_DISKONLY).					*
 ************************************************************************
 */
#define	DLB_DEVICES	0L	/* Return devices				*/
#define	DLB_DISKONLY	1L	/* Modifier for above: Return disk devices only	*/
#define	DLB_VOLUMES	2L	/* Return volumes only				*/
#define	DLB_DIRS	3L	/* Return assigned devices only			*/

#define	DLF_DEVICES	(1L << DLB_DEVICES)
#define	DLF_DISKONLY	(1L << DLB_DISKONLY)
#define	DLF_VOLUMES	(1L << DLB_VOLUMES)
#define	DLF_DIRS	(1L << DLB_DIRS)

/*
 ************************************************************************
 * Legal de_Type values, check for these after a call to AddDADevs(),	*
 * or use on your own as the ID values in AddDANode().			*
 ************************************************************************
 */
#define	DLX_FILE	0L	/* AddDADevs() can't determine this	*/
#define	DLX_DIR		8L	/* AddDADevs() can't determine this	*/
#define	DLX_DEVICE	16L	/* It's a resident device		*/

#define	DLX_VOLUME	24L	/* Device is a volume			*/
#define	DLX_UNMOUNTED	32L	/* Device is not resident		*/

#define	DLX_ASSIGN	40L	/* Device is a logical assignment	*/

/*
 ************************************************************************
 *	This macro is to check for an error return from the Atol()	*
 *	routine.  If Errno is ERRBADINT, then there was an error...	*
 *	This was done to try to remain as close to source compatible	*
 *	as possible with the older (rel 1.1) ARPbase.h			*
 ************************************************************************
 */
#define	ERRBADINT	1L
#define	Errno		(IoErr() ? ERRBADINT : 0)

/*
 ************************************************************************
 *	Resource Tracking stuff...					*
 ************************************************************************
 *									*
 * There are a few things in arp.library that are only directly		*
 * acessable from assembler.  The glue routines provided by us for	*
 * all 'C' compilers use the following conventions to make these	*
 * available to C programs.  The glue for other language's should use	*
 * as similar a mechanism as possible, so that no matter what language	*
 * or compiler we speak, when talk about arp, we will know what the	*
 * other guy is saying.							*
 *									*
 * Here are the cases:							*
 *									*
 * Tracker calls...							*
 *		These calls return the Tracker pointer as a secondary	*
 *		result in the register A1.  For C, there is no clean	*
 *		way to return more than one result so the tracker	*
 *		pointer is returned in IoErr().  For ease of use,	*
 *		there is a define that typecasts IoErr() to the correct	*
 *		pointer type.  This is called LastTracker and should	*
 *		be source compatible with the earlier method of storing	*
 *		the secondary result.					*
 *									*
 * GetTracker() -							*
 *		Syntax is a bit different for C than the assembly call	*
 *		The C syntax is GetTracker(ID).  The binding routines	*
 *		will store the ID into the tracker on return.  Also,	*
 *		in an effort to remain consistant, the tracker will	*
 *		also be stored in LastTracker.				*
 *									*
 * In cases where you have allocated a tracker before you have obtained	*
 * a resource (usually the most efficient method), and the resource has	*
 * not been obtained, you will need to clear the tracker id.  The macro	*
 * CLEAR_ID() has been provided for that purpose.  It expects a pointer	*
 * to a DefaultTracker sort of struct.					*
 ************************************************************************
 */
#define	CLEAR_ID(t)	((SHORT *) t)[-1]=NULL

/*
 ************************************************************************
 * You MUST prototype IoErr() to prevent the possible error in defining	*
 * IoErr() and thus causing LastTracker to give you trash...		*
 *									*
 * N O T E !  You MUST! have IoErr() defined as LONG to use LastTracker *
 *	      If your compiler has other defines for this, you may wish *
 *	      to remove the prototype for IoErr().			*
 ************************************************************************
 */
#define	LastTracker	((struct DefaultTracker *)IoErr())

/*
 ************************************************************************
 * The rl_FirstItem list (ResList) is a list of TrackedResource (below)	*
 * It is very important that nothing in this list depend on the task	*
 * existing at resource freeing time (i.e., RemTask(0L) type stuff,	*
 * DeletePort() and the rest).						*
 *									*
 * The tracking functions return a struct Tracker *Tracker to you, this	*
 * is a pointer to whatever follows the tr_ID variable.			*
 * The default case is reflected below, and you get it if you call	*
 * GetTracker() ( see DefaultTracker below).				*
 *									*
 * NOTE: The two user variables mentioned in an earlier version don't	*
 * exist, and never did. Sorry about that (SDB).			*
 *									*
 * However, you can still use ArpAlloc() to allocate your own tracking	*
 * nodes and they can be any size or shape you like, as long as the	*
 * base structure is preserved. They will be freed automagically just	*
 * like the default trackers.						*
 ************************************************************************
 */
struct	TrackedResource	{
		struct	MinNode	tr_Node;	/* Double linked pointer		*/
			BYTE	tr_Flags;	/* Don't touch				*/
			BYTE	tr_Lock;	/* Don't touch, for Get/FreeAccess()	*/
			SHORT	tr_ID;		/* Item's ID				*/

/*
 ************************************************************************
 * The struct DefaultTracker *Tracker portion of the structure.		*
 * The stuff below this point can conceivably vary, depending		*
 * on user needs, etc.  This reflects the default.			*
 ************************************************************************
 */
			union	{
				CPTR	tr_Resource;	/* Whatever				*/
        			LONG	tg_Verify;	/* For use during TRAK_GENERIC		*/
				}	tr_Object;	/* The thing being tracked		*/
			union	{
				VOID	(*tg_Function)();/* Function to call for TRAK_GENERIC	*/
			struct	Window	*tr_Window2;	/* For TRAK_WINDOW			*/
				}	tr_Extra;	/* Only needed sometimes		*/
			};

#define	tg_Value tg_Verify	/* Ancient compatibility only!  Do NOT use in new CODE!!! */

/*
 ************************************************************************
 * You get a pointer to a struct of the following type when you call	*
 * GetTracker().  You can change this, and use ArpAlloc() instead of	*
 * GetTracker() to do tracking. Of course, you have to take a wee bit	*
 * more responsibility if you do, as well as if you use TRAK_GENERIC	*
 * stuff.								*
 *									*
 * TRAK_GENERIC folks need to set up a task function to be called when	*
 * an item is freed.  Some care is required to set this up properly.	*
 *									*
 * Some special cases are indicated by the unions below, for		*
 * TRAK_WINDOW, if you have more than one window opened, and don't	*
 * want the IDCMP closed particularly, you need to set a ptr to the	*
 * other window in dt_Window2.  See CloseWindowSafely() for more info.	*
 * If only one window, set this to NULL.				*
 ************************************************************************
 */
struct	DefaultTracker	{
			union	{
				CPTR	dt_Resource;	/* Whatever				*/
				LONG	tg_Verify;	/* For use during TRAK_GENERIC		*/
				}	dt_Object;	/* The object being tracked		*/
			union	{
				VOID	(*tg_Function)();/* Function to call for TRAK_GENERIC	*/
			struct	Window	*dt_Window2;	/* For TRAK_WINDOW			*/
				}	dt_Extra;
			};

/*
 ************************************************************************
 *	Items the tracker knows what to do about			*
 ************************************************************************
 */
#define	TRAK_AAMEM	0L	/* Default (ArpAlloc) element		*/
#define	TRAK_LOCK	1L	/* File lock				*/
#define	TRAK_FILE	2L	/* Opened file				*/
#define	TRAK_WINDOW	3L	/* Window -- see docs			*/
#define	TRAK_SCREEN	4L	/* Screen				*/
#define	TRAK_LIBRARY	5L	/* Opened library			*/
#define	TRAK_DAMEM	6L	/* Pointer to DosAllocMem block		*/
#define	TRAK_MEMNODE	7L	/* AllocEntry() node			*/
#define	TRAK_SEGLIST	8L	/* Program segment			*/
#define	TRAK_RESLIST	9L	/* ARP (nested) ResList			*/
#define	TRAK_MEM	10L	/* Memory ptr/length			*/
#define	TRAK_GENERIC	11L	/* Generic Element, your choice		*/
#define	TRAK_DALIST	12L	/* DAlist ( aka file request )		*/
#define	TRAK_ANCHOR	13L	/* Anchor chain (pattern matching)	*/
#define	TRAK_FREQ	14L	/* FileRequest struct			*/
#define	TRAK_FONT	15L	/* GfxBase CloseFont()			*/
#define	TRAK_MAX	15L	/* Poof, anything higher is tossed	*/

#define	TRB_UNLINK	7L	/* Free node bit			*/
#define	TRB_RELOC	6L	/* This may be relocated (not used yet)	*/
#define	TRB_MOVED	5L	/* Item moved				*/

#define	TRF_UNLINK	(1L << TRB_UNLINK)
#define	TRF_RELOC	(1L << TRB_RELOC)
#define	TRF_MOVED	(1L << TRB_MOVED)

/*
 ************************************************************************
 * Note: ResList MUST be a DosAllocMem'ed list!, this is done for	*
 * you when you call CreateTaskResList(), typically, you won't need	*
 * to access/allocate this structure.					*
 ************************************************************************
 */
struct	ResList	{
	struct	MinNode		rl_Node;	/* Used by arplib to link reslists	*/
	struct	Task		*rl_TaskID;	/* Owner of this list			*/
	struct	MinList		rl_FirstItem;	/* List of Tracked Resources		*/
	struct	ResList		*rl_Link;	/* SyncRun's use - hide list here	*/
		};

/*
 ************************************************************************
 *	Returns from CompareLock()					*
 ************************************************************************
 */
#define	LCK_EQUAL	0L	/* The two locks refer to the same object	*/
#define	LCK_VOLUME	1L	/* Locks are on the same volume			*/
#define	LCK_DIFVOL1	2L	/* Locks are on different volumes		*/
#define	LCK_DIFVOL2	3L	/* Locks are on different volumes		*/

/*
 ************************************************************************
 *	ASyncRun() stuff...						*
 ************************************************************************
 * Message sent back on your request by an exiting process.		*
 * You request this by putting the address of your message in		*
 * pcb_LastGasp, and initializing the ReplyPort variable of your	*
 * ZombieMsg to the port you wish the message posted to.		*
 ************************************************************************
 */
struct	ZombieMsg	{
		struct	Message		zm_ExecMessage;
			ULONG		zm_TaskNum;	/* Task ID			*/
			LONG		zm_ReturnCode;	/* Process's return code	*/
			ULONG		zm_Result2;	/* System return code		*/
		struct	DateStamp	zm_ExitTime;	/* Date stamp at time of exit	*/
			ULONG		zm_UserInfo;	/* For whatever you wish	*/
			};

/*
 ************************************************************************
 * Structure required by ASyncRun() -- see docs for more info.		*
 ************************************************************************
 */
struct	ProcessControlBlock	{
				ULONG		pcb_StackSize;	/* Stacksize for new process			*/
				BYTE		pcb_Pri;	/* Priority of new task				*/
				UBYTE		pcb_Control;	/* Control bits, see defines below		*/
				APTR		pcb_TrapCode;	/* Optional Trap Code				*/
				BPTR		pcb_Input;
				BPTR		pcb_Output;	/* Optional stdin, stdout			*/
				union	{
					BPTR	pcb_SplatFile;	/* File to use for Open("*")			*/
					BYTE	*pcb_ConName;	/* CON: filename				*/
					}	pcb_Console;
				CPTR		pcb_LoadedCode;	/* If not null, will not load/unload code	*/
			struct	ZombieMsg	*pcb_LastGasp;	/* ReplyMsg() to be filled in by exit		*/
			struct	MsgPort		*pcb_WBProcess;	/* Valid only when PRB_NOCLI			*/
				};

/*
 ************************************************************************
 * Formerly needed to pass NULLCMD to a child.  No longer needed.	*
 * It is being kept here for compatibility only...			*
 ************************************************************************
 */
#define	NOCMD	"\n"

/*
 ************************************************************************
 * The following control bits determine what ASyncRun() does on		*
 * Abnormal Exits and on background process termination.		*
 ************************************************************************
 */
#define	PRB_SAVEIO	0L	/* Don't free/check file handles on exit	*/
#define	PRB_CLOSESPLAT	1L	/* Close Splat file, must request explicitly	*/
#define	PRB_NOCLI	2L	/* Don't create a CLI process			*/
/*	PRB_INTERACTIVE	3L	   This is now obsolete...			*/
#define	PRB_CODE	4L	/* Dangerous yet enticing			*/
#define	PRB_STDIO	5L	/* Do the stdio thing, splat = CON:Filename 	*/

#define	PRF_SAVEIO	(1L << PRB_SAVEIO)
#define	PRF_CLOSESPLAT	(1L << PRB_CLOSESPLAT)
#define	PRF_NOCLI	(1L << PRB_NOCLI)
#define	PRF_CODE	(1L << PRB_CODE)
#define	PRF_STDIO	(1L << PRB_STDIO)

/*
 ************************************************************************
 *	Error returns from SyncRun() and ASyncRun()			*
 ************************************************************************
 */
#define	PR_NOFILE	-1L	/* Could not LoadSeg() the file			*/
#define	PR_NOMEM	-2L	/* No memory for something			*/
/*	PR_NOCLI	-3L	   This is now obsolete				*/
#define	PR_NOSLOT	-4L	/* No room in TaskArray				*/
#define	PR_NOINPUT	-5L	/* Could not open input file			*/
#define	PR_NOOUTPUT	-6L	/* Could not get output file			*/
/*	PR_NOLOCK	-7L	   This is now obsolete				*/
/*	PR_ARGERR	-8L	   This is now obsolete				*/
/*	PR_NOBCPL	-9L	   This is now obsolete				*/
/*	PR_BADLIB	-10L	   This is now obsolete				*/
#define	PR_NOSTDIO	-11L	/* Couldn't get stdio handles			*/

/*
 ************************************************************************
 *	Added V35 of arp.library					*
 ************************************************************************
 */
#define	PR_WANTSMESSAGE	-12L	/* Child wants you to report IoErr() to user	*/
				/* for SyncRun() only...			*/
#define	PR_NOSHELLPROC	-13L	/* Can't create a shell/cli process		*/
#define	PR_NOEXEC	-14L	/* 'E' bit is clear				*/
#define	PR_SCRIPT	-15L	/* S and E are set, IoErr() contains directory	*/

/*
 ************************************************************************
 * Version 35 ASyncRun() allows you to create an independent		*
 * interactive or background Shell/CLI. You need this variant of the	*
 * pcb structure to do it, and you also have new values for nsh_Control,*
 * see below.								*
 *									*
 * Syntax for Interactive shell is:					*
 *									*
 * rc=ASyncRun("Optional Window Name","Optional From File",&NewShell);	*
 *									*
 * Syntax for a background shell is:					*
 *									*
 * rc=ASyncRun("Command line",0L,&NewShell);				*
 *									*
 * Same syntax for an Execute style call, but you have to be on drugs	*
 * if you want to do that.						*
 ************************************************************************
 */
struct	NewShell	{
			ULONG	nsh_StackSize;	/* stacksize shell will use for children	*/
			BYTE	nsh_Pri;	/* ignored by interactive shells		*/
			UBYTE	nsh_Control;	/* bits/values: see above			*/
			CPTR	nsh_LogMsg;	/* Optional login message, if null, use default	*/
			BPTR	nsh_Input;	/* ignored by interactive shells, but		*/
			BPTR	nsh_Output;	/* used by background and execute options.	*/
			LONG	nsh_RESERVED[5];
			};

/*
 ************************************************************************
 * Bit Values for nsh_Control, you should use them as shown below, or	*
 * just use the actual values indicated.				*
 ************************************************************************
 */
#define	PRB_CLI		0L	/* Do a CLI, not a shell	*/
#define	PRB_BACKGROUND	1L	/* Background shell		*/
#define	PRB_EXECUTE	2L	/* Do as EXECUTE...		*/
#define	PRB_INTERACTIVE	3L	/* Run an interactive shell	*/
#define	PRB_FB		7L	/* Alt function bit...		*/

#define	PRF_CLI		(1L << PRB_CLI)
#define	PRF_BACKGOUND	(1L << PRB_BACKGROUND)
#define	PRF_EXECUTE	(1L << PRB_EXECUTE)
#define	PRF_INTERACTIVE	(1L << PRB_INTERACTIVE)
#define	PRF_FB		(1L << PRB_FB)

/*
 ************************************************************************
 *	Common values for sh_Control which allow you to do usefull	*
 *	and somewhat "standard" things...				*
 ************************************************************************
 */
#define	INTERACTIVE_SHELL	(PRF_FB|PRF_INTERACTIVE)		/* Gimme a newshell!		*/
#define	INTERACTIVE_CLI		(PRF_FB|PRF_INTERACTIVE|PRF_CLI)	/* Gimme that ol newcli!	*/
#define	BACKGROUND_SHELL	(PRF_FB|PRF_BACKGROUND)			/* gimme a background shell	*/
#define	EXECUTE_ME		(PRF_FB|PRF_BACKGROUND|PRF_EXECUTE)	/* aptly named, doncha think?	*/

/*
 ************************************************************************
 *	Additional IoErr() returns added by ARP...			*
 ************************************************************************
 */
#define	ERROR_NOT_CLI		400L	/* Program/function neeeds to be cli	*/

/*
 ************************************************************************
 *	Resident Program Support					*
 ************************************************************************
 * This is the kind of node allocated for you when you AddResidentPrg()	*
 * a code segment.  They are stored as a single linked list with the	*
 * root in ArpBase.  If you absolutely *must* wander through this list	*
 * instead of using the supplied functions, then you must first obtain	*
 * the semaphore which protects this list, and then release it		*
 * afterwards.  Do not use Forbid() and Permit() to gain exclusive	*
 * access!  Note that the supplied functions handle this locking	*
 * protocol for you.							*
 ************************************************************************
 */
struct	ResidentProgramNode	{
			struct	ResidentProgramNode	*rpn_Next;	/* next or NULL			*/
				LONG			rpn_Usage;	/* Number of current users	*/
				UWORD			rpn_AccessCnt;	/* Total times used...		*/
				ULONG			rpn_CheckSum;	/* Checksum of code		*/
				BPTR			rpn_Segment;	/* Actual segment		*/
				UWORD			rpn_Flags;	/* See definitions below...	*/
				BYTE			rpn_Name[1];	/* Allocated as needed		*/
				};

/*
 ************************************************************************
 *	Bit definitions for rpn_Flags....				*
 ************************************************************************
 */
#define	RPNB_NOCHECK	0L	/* Set in rpn_Flags for no checksumming...	*/
#define	RPNB_CACHE	1L	/* Private usage in v1.3...			*/

#define	RPNF_NOCHECK	(1L << RPNB_NOCHECK)
#define	RPNF_CACHE	(1L << RPNB_CACHE)

/*
 ************************************************************************
 * If your program starts with this structure, ASyncRun() and SyncRun()	*
 * will override a users stack request with the value in rpt_StackSize.	*
 * Furthermore, if you are actually attached to the resident list, a	*
 * memory block of size rpt_DataSize will be allocated for you, and	*
 * a pointer to this data passed to you in register A4.  You may use	*
 * this block to clone the data segment of programs, thus resulting in	*
 * one copy of text, but multiple copies of data/bss for each process	*
 * invocation.  If you are resident, your program will start at		*
 * rpt_Instruction, otherwise, it will be launched from the initial	*
 * branch.								*
 ************************************************************************
 */
struct	ResidentProgramTag	{
				BPTR	rpt_NextSeg;	/* Provided by DOS at LoadSeg time	*/
/*
 ************************************************************************
 * The initial branch destination and rpt_Instruction do not have to be	*
 * the same.  This allows different actions to be taken if you are	*
 * diskloaded or resident.  DataSize memory will be allocated only if	*
 * you are resident, but StackSize will override all user stack		*
 * requests.								*
 ************************************************************************
 */
				UWORD	rpt_BRA;	/* Short branch to executable		*/
				UWORD	rpt_Magic;	/* Resident majik value			*/
				ULONG	rpt_StackSize;	/* min stack for this process		*/
				ULONG	rpt_DataSize;	/* Data size to allocate if resident	*/
				/*	rpt_Instruction;	Start here if resident		*/
				};

/*
 ************************************************************************
 * The form of the ARP allocated node in your tasks memlist when	*
 * launched as a resident program. Note that the data portion of the	*
 * node will only exist if you have specified a nonzero value for	*
 * rpt_DataSize. Note also that this structure is READ ONLY, modify	*
 * values in this at your own risk.  The stack stuff is for tracking,	*
 * if you need actual addresses or stack size, check the normal places	*
 * for it in your process/task struct.					*
 ************************************************************************
 */
struct	ProcessMemory	{
		struct	Node	pm_Node;
			UWORD	pm_Num;		/* This is 1 if no data, two if data	*/
			CPTR	pm_Stack;
			ULONG	pm_StackSize;
			CPTR	pm_Data;	/* Only here if pm_Num == 2		*/
			ULONG	pm_DataSize;
			};

/*
 ************************************************************************
 * To find the above on your memlist, search for the following name.	*
 * We guarantee this will be the only arp.library allocated node on	*
 * your memlist with this name.						*
 * i.e. FindName(task->tcb_MemEntry, PMEM_NAME);			*
 ************************************************************************
 */
#define	PMEM_NAME	"ARP_MEM"

#define	RESIDENT_MAGIC	0x4AFC		/* same as RTC_MATCHWORD (trapf) */

/*
 ************************************************************************
 *	Date String/Data structures					*
 ************************************************************************
 */
#ifndef DOS_DATETIME_H		/* added by mool */
#define DOS_DATETIME_H

struct	DateTime	{
		struct	DateStamp	dat_Stamp;	/* DOS Datestamp			*/
			UBYTE		dat_Format;	/* controls appearance ot dat_StrDate	*/
			UBYTE		dat_Flags;	/* See BITDEF's below			*/
			BYTE		*dat_StrDay;	/* day of the week string		*/
			BYTE		*dat_StrDate;	/* date string				*/
			BYTE		*dat_StrTime;	/* time string				*/
			};

/*
 ************************************************************************
 *	Size of buffer you need for each DateTime strings:		*
 ************************************************************************
 */
#define	LEN_DATSTRING	10L

/*
 ************************************************************************
 *	For dat_Flags							*
 ************************************************************************
 */
#define	DTB_SUBST	0L	/* Substitute "Today" "Tomorrow" where appropriate	*/
#define	DTB_FUTURE	1L	/* Day of the week is in future				*/

#define	DTF_SUBST	(1L << DTB_SUBST)
#define	DTF_FUTURE	(1L << DTB_FUTURE)

/*
 ************************************************************************
 *	For dat_Format							*
 ************************************************************************
 */
#define	FORMAT_DOS	0L		/* dd-mmm-yy AmigaDOS's own, unique style		*/
#define	FORMAT_INT	1L		/* yy-mm-dd International format			*/
#define	FORMAT_USA	2L		/* mm-dd-yy The good'ol'USA.				*/
#define	FORMAT_CDN	3L		/* dd-mm-yy Our brothers and sisters to the north	*/
#define	FORMAT_MAX	FORMAT_CDN	/* Larger than this? Defaults to AmigaDOS		*/

	/* dos/datetime.h uses the same structures and defines, so
	 * keep the compiler from pulling it in. -olsen
	 */

/* #define DOS_DATETIME_H 1 */
#endif

/*
 ************************************************************************
 * Define NO_PROTOTYPES if your compiler does not handle them...	*
 ************************************************************************
 */
#if defined(NO_PROTOTYPES) || defined(__NO_PROTOS)
#define	ARGs(x)	()
#else
#define	ARGs(x)	x

	/* Added ArpVPrintf, ArpVFPrintf and ArpVSPrintf, so will have to
	 * include the compiler specific stdarg header file. -olsen
	 */

#include <stdarg.h>

#endif	/* NO_PROTOTYPES */

/*
 ************************************************************************
 * Note that C_Args is a #define that, in LATTICE does __stdargs	*
 ************************************************************************
 */

/*
 ************************************************************************
 * This prototype is here to prevent the possible error in defining	*
 * IoErr() as LONG and thus causing LastTracker to give you trash...	*
 *									*
 * N O T E !  You MUST! have IoErr() defined as LONG to use LastTracker *
 *	      If your compiler has other defines for this, you may wish *
 *	      to move the prototype for IoErr() into the DO_ARP_COPIES	*
 ************************************************************************
 */
	LONG			IoErr			ARGs(	(VOID)							);

/*
 ************************************************************************
 *	These duplicate the calls in dos.library			*
 *	Only include if you can use arp.library without dos.library	*
 ************************************************************************
 */
#ifdef	DO_ARP_COPIES
	BPTR			Open			ARGs(	(char *, LONG)						);
	VOID			Close			ARGs(	(BPTR)							);
	LONG			Read			ARGs(	(BPTR, char *, LONG)					);
	LONG			Write			ARGs(	(BPTR, char *, LONG)					);
	BPTR			Input			ARGs(	(VOID)							);
	BPTR			Output			ARGs(	(VOID)							);
	LONG			Seek			ARGs(	(BPTR, LONG, LONG)					);
	LONG			DeleteFile		ARGs(	(char *)						);
	LONG			Rename			ARGs(	(char *, char *)					);
	BPTR			Lock			ARGs(	(char *, LONG)						);
	VOID			UnLock			ARGs(	(BPTR)							);
	BPTR			DupLock			ARGs(	(BPTR)							);
	LONG			Examine			ARGs(	(BPTR, struct FileInfoBlock *)				);
	LONG			ExNext			ARGs(	(BPTR, struct FileInfoBlock *)				);
	LONG			Info			ARGs(	(BPTR, struct InfoData *)				);
	BPTR			CreateDir		ARGs(	(char *)						);
	BPTR			CurrentDir		ARGs(	(BPTR)							);
struct	MsgPort			*CreateProc		ARGs(	(char *, LONG, BPTR, LONG)				);
	VOID			Exit			ARGs(	(LONG)							);
	BPTR			LoadSeg			ARGs(	(char *)						);
	VOID			UnLoadSeg		ARGs(	(BPTR)							);
struct	MsgPort			*DeviceProc		ARGs(	(char *)						);
	LONG			SetComment		ARGs(	(char *, char *)					);
	LONG			SetProtection		ARGs(	(char *, LONG)						);
	LONG			*DateStamp		ARGs(	(LONG *)						);
	VOID			Delay			ARGs(	(LONG)							);
	LONG			WaitForChar		ARGs(	(BPTR, LONG)						);
	BPTR			ParentDir		ARGs(	(BPTR)							);
	LONG			IsInteractive		ARGs(	(BPTR)							);
	LONG			Execute			ARGs(	(char *, BPTR, BPTR)					);
#endif	/* DO_ARP_COPIES */

/*
 ************************************************************************
 *	Now for the stuff that only exists in arp.library...		*
 ************************************************************************
 */
	/* LONG		C_Args	Printf			ARGs(	(char *,...)						); */
	/* LONG		C_Args	FPrintf			ARGs(	(BPTR, char *,...)					); */
#ifdef SASC
	LONG		C_Args	Printf			ARGs(	(UBYTE *,...)						);
	LONG		C_Args	FPrintf			ARGs(	(BPTR, UBYTE *,...)					);
#else
	LONG		C_Args	Printf			ARGs(	(UBYTE *, long, ...)						);
	LONG		C_Args	FPrintf			ARGs(	(BPTR, UBYTE *, long, ...)					);
#endif
	LONG			Puts 			ARGs(	(char *)						);
	LONG			ReadLine		ARGs(	(char *)						);
	LONG			GADS		 	ARGs(	(char *, LONG, char *, char **, char *)			);
	LONG			Atol			ARGs(	(char *)						);
	ULONG			EscapeString 		ARGs(	(char *)						);
	LONG			CheckAbort		ARGs(	(VOID(*))						);
	LONG			CheckBreak		ARGs(	(LONG, VOID(*))						);
	BYTE			*Getenv			ARGs(	(char *, char *, LONG)					);
	BOOL			Setenv			ARGs(	(char *, char *)					);
	BYTE			*FileRequest 		ARGs(	(struct FileRequester *)				);
	VOID			CloseWindowSafely	ARGs(	(struct Window *, LONG)					);
/* struct	MsgPort			*CreatePort		ARGs(	(const char *, LONG)						); */
struct	MsgPort			*CreatePort		ARGs(	(UBYTE *, LONG)						);
	VOID			DeletePort 		ARGs(	(struct MsgPort *)					);
	LONG			SendPacket 		ARGs(	(LONG, LONG *, struct MsgPort *)			);
	VOID			InitStdPacket		ARGs(	(LONG, LONG *, struct DosPacket *, struct MsgPort *)	);
	ULONG			PathName		ARGs(	(BPTR, char *,LONG)					);
	ULONG			Assign			ARGs(	(char *, char *)					);
	VOID			*DosAllocMem		ARGs(	(LONG)							);
	VOID			DosFreeMem		ARGs(	(VOID *)						);
	ULONG			BtoCStr			ARGs(	(char *, BSTR, LONG)					);
	ULONG			CtoBStr			ARGs(	(char *, BSTR, LONG)					);
struct	DeviceList		*GetDevInfo		ARGs(	(struct DeviceList *)					);
	BOOL			FreeTaskResList		ARGs(	(VOID)							);
	VOID			ArpExit 		ARGs(	(LONG,LONG)						);
	VOID		C_Args	*ArpAlloc		ARGs(	(LONG)							);
	VOID		C_Args	*ArpAllocMem		ARGs(	(LONG, LONG)						);
	BPTR		C_Args	ArpOpen			ARGs(	(char *, LONG)						);
	BPTR		C_Args	ArpDupLock		ARGs(	(BPTR)							);
	BPTR		C_Args	ArpLock			ARGs(	(char *, LONG)						);
	VOID		C_Args	*RListAlloc		ARGs(	(struct ResList *, LONG)				);
struct	Process			*FindCLI		ARGs(	(LONG)							);
	BOOL			QSort			ARGs(	(VOID *, LONG, LONG, int(*))				);
	BOOL			PatternMatch		ARGs(	(char *,char *)						);
	LONG			FindFirst		ARGs(	(char *, struct AnchorPath *)				);
	LONG			FindNext		ARGs(	(struct AnchorPath *)					);
	VOID			FreeAnchorChain		ARGs(	(struct AnchorPath *)					);
	ULONG			CompareLock		ARGs(	(BPTR, BPTR)						);
struct	ResList			*FindTaskResList	ARGs(	(VOID)							);
struct	ResList			*CreateTaskResList	ARGs(	(VOID)							);
	VOID			FreeResList		ARGs(	(struct ResList *)					);
	VOID			FreeTrackedItem		ARGs(	(struct DefaultTracker *)				);
struct	DefaultTracker	C_Args	*GetTracker		ARGs(	(LONG)							);
	VOID			*GetAccess		ARGs(	(struct DefaultTracker *)				);
	VOID			FreeAccess		ARGs(	(struct DefaultTracker *)				);
	VOID			FreeDAList		ARGs(	(struct DirectoryEntry *)				);
struct	DirectoryEntry		*AddDANode		ARGs(	(char *, struct DirectoryEntry **, LONG, LONG)		);
	ULONG			AddDADevs		ARGs(	(struct DirectoryEntry **, LONG)			);
	LONG			Strcmp			ARGs(	(char *, char *)					);
	LONG			Strncmp			ARGs(	(char *, char *, LONG)					);
	BYTE			Toupper			ARGs(	(BYTE)							);
	LONG			SyncRun			ARGs(	(char *, char *, BPTR, BPTR)				);

/*
 ************************************************************************
 *	Added V32 of arp.library					*
 ************************************************************************
 */
	LONG			ASyncRun		ARGs(	(char *, char *, struct ProcessControlBlock *)		);
	LONG			SpawnShell		ARGs(	(char *, char *, struct NewShell *)			);
	BPTR			LoadPrg			ARGs(	(char *)						);
	BOOL			PreParse		ARGs(	(char *, char *)					);

/*
 ************************************************************************
 *	Added V33 of arp.library					*
 ************************************************************************
 */
	BOOL			StamptoStr		ARGs(	(struct DateTime *)					);
	BOOL			StrtoStamp		ARGs(	(struct DateTime *)					);
struct	ResidentProgramNode	*ObtainResidentPrg	ARGs(	(char *)						);
struct	ResidentProgramNode	*AddResidentPrg		ARGs(	(BPTR, char *)						);
	LONG			RemResidentPrg		ARGs(	(char *)						);
	VOID			UnLoadPrg		ARGs(	(BPTR)							);
	LONG			LMult			ARGs(	(LONG, LONG)						);
	LONG			LDiv			ARGs(	(LONG, LONG)						);
	LONG			LMod			ARGs(	(LONG, LONG)						);
	ULONG			CheckSumPrg		ARGs(	(struct ResidentProgramNode *)				);
	VOID			TackOn			ARGs(	(char *, char *)					);
	BYTE			*BaseName		ARGs(	(char *)						);
struct	ResidentProgramNode	*ReleaseResidentPrg	ARGs(	(BPTR)							);

/*
 ************************************************************************
 *	Added V36 of arp.library					*
 ************************************************************************
 */
	LONG		C_Args	SPrintf			ARGs(	(char *, char *,...)					);
	LONG			GetKeywordIndex		ARGs(	(char *, char *)					);
struct	Library		C_Args	*ArpOpenLibrary		ARGs(	(char *, LONG)						);
struct	FileRequester	C_Args	*ArpAllocFreq		ARGs(	(VOID)							);

	/* This one's a cutie which is supported via bypassing the
	 * ??Printf glue routines. -olsen
	 */

	LONG			ArpVPrintf		ARGs(	(char *, va_list)					);
	LONG			ArpVFPrintf		ARGs(	(BPTR, char *, va_list)					);
	LONG			ArpVSPrintf		ARGs(	(char *, char *, va_list)				);

/*
 ************************************************************************
 *	Check if we should do the pragmas...				*
 ************************************************************************
 */

#if !defined(NO_PRAGMAS) && !defined(__NO_PRAGMAS)
#ifndef	PROTO_ARP_H
#include <proto/arp.h>
#endif	/* PROTO_ARP_H */
#endif	/* NO_PRAGMAS */

#endif	/* LIBRARIES_ARPBASE_H */
