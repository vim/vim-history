/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.03.0110 */
/* at Mon Feb 01 19:13:42 1999
 */
/* Compiler settings for D:\Src2\VisVim\VisVim.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __VSVTypes_h__
#define __VSVTypes_h__

#ifdef __cplusplus
extern "C"{
#endif

/* Forward Declarations */

#ifndef __ICommands_FWD_DEFINED__
#define __ICommands_FWD_DEFINED__
typedef interface ICommands ICommands;
#endif	/* __ICommands_FWD_DEFINED__ */


#ifndef __Commands_FWD_DEFINED__
#define __Commands_FWD_DEFINED__

#ifdef __cplusplus
typedef class Commands Commands;
#else
typedef struct Commands Commands;
#endif /* __cplusplus */

#endif	/* __Commands_FWD_DEFINED__ */


#ifndef __ApplicationEvents_FWD_DEFINED__
#define __ApplicationEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class ApplicationEvents ApplicationEvents;
#else
typedef struct ApplicationEvents ApplicationEvents;
#endif /* __cplusplus */

#endif	/* __ApplicationEvents_FWD_DEFINED__ */


#ifndef __DebuggerEvents_FWD_DEFINED__
#define __DebuggerEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class DebuggerEvents DebuggerEvents;
#else
typedef struct DebuggerEvents DebuggerEvents;
#endif /* __cplusplus */

#endif	/* __DebuggerEvents_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * );


#ifndef __VisVim_LIBRARY_DEFINED__
#define __VisVim_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: VisVim
 * at Mon Feb 01 19:13:42 1999
 * using MIDL 3.03.0110
 ****************************************/
/* [helpstring][version][uuid] */



DEFINE_GUID(LIBID_VisVim,0xAC726707,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#ifndef __ICommands_INTERFACE_DEFINED__
#define __ICommands_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICommands
 * at Mon Feb 01 19:13:42 1999
 * using MIDL 3.03.0110
 ****************************************/
/* [object][dual][oleautomation][uuid] */



DEFINE_GUID(IID_ICommands,0xAC726703,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("AC726703-2977-11D1-B2F3-006008040780")
    ICommands : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE VisVimDialog( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE VisVimEnable( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE VisVimDisable( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE VisVimToggle( void) = 0;

        virtual HRESULT STDMETHODCALLTYPE VisVimLoad( void) = 0;

    };

#else	/* C style interface */

    typedef struct ICommandsVtbl
    {
        BEGIN_INTERFACE

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )(
            ICommands __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )(
            ICommands __RPC_FAR * This);

        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )(
            ICommands __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )(
            ICommands __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )(
            ICommands __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )(
            ICommands __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);

        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )(
            ICommands __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);

        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimDialog )(
            ICommands __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimEnable )(
            ICommands __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimDisable )(
            ICommands __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimToggle )(
            ICommands __RPC_FAR * This);

        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimLoad )(
            ICommands __RPC_FAR * This);

        END_INTERFACE
    } ICommandsVtbl;

    interface ICommands
    {
        CONST_VTBL struct ICommandsVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define ICommands_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICommands_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICommands_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICommands_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICommands_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICommands_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICommands_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICommands_VisVimDialog(This)	\
    (This)->lpVtbl -> VisVimDialog(This)

#define ICommands_VisVimEnable(This)	\
    (This)->lpVtbl -> VisVimEnable(This)

#define ICommands_VisVimDisable(This)	\
    (This)->lpVtbl -> VisVimDisable(This)

#define ICommands_VisVimToggle(This)	\
    (This)->lpVtbl -> VisVimToggle(This)

#define ICommands_VisVimLoad(This)	\
    (This)->lpVtbl -> VisVimLoad(This)

#endif /* COBJMACROS */


#endif	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ICommands_VisVimDialog_Proxy(
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICommands_VisVimEnable_Proxy(
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimEnable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICommands_VisVimDisable_Proxy(
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimDisable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICommands_VisVimToggle_Proxy(
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimToggle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICommands_VisVimLoad_Proxy(
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimLoad_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif	/* __ICommands_INTERFACE_DEFINED__ */


DEFINE_GUID(CLSID_Commands,0xAC726704,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#ifdef __cplusplus

class DECLSPEC_UUID("AC726704-2977-11D1-B2F3-006008040780")
Commands;
#endif

DEFINE_GUID(CLSID_ApplicationEvents,0xAC726705,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#ifdef __cplusplus

class DECLSPEC_UUID("AC726705-2977-11D1-B2F3-006008040780")
ApplicationEvents;
#endif

DEFINE_GUID(CLSID_DebuggerEvents,0xAC726706,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#ifdef __cplusplus

class DECLSPEC_UUID("AC726706-2977-11D1-B2F3-006008040780")
DebuggerEvents;
#endif
#endif /* __VisVim_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
