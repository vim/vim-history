/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Feb 13 03:01:38 1998
 */
/* Compiler settings for D:\Src2\VisVim\VisVIM.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
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
#endif 	/* __ICommands_FWD_DEFINED__ */


#ifndef __Commands_FWD_DEFINED__
#define __Commands_FWD_DEFINED__

#ifdef __cplusplus
typedef class Commands Commands;
#else
typedef struct Commands Commands;
#endif /* __cplusplus */

#endif 	/* __Commands_FWD_DEFINED__ */


#ifndef __ApplicationEvents_FWD_DEFINED__
#define __ApplicationEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class ApplicationEvents ApplicationEvents;
#else
typedef struct ApplicationEvents ApplicationEvents;
#endif /* __cplusplus */

#endif 	/* __ApplicationEvents_FWD_DEFINED__ */


#ifndef __DebuggerEvents_FWD_DEFINED__
#define __DebuggerEvents_FWD_DEFINED__

#ifdef __cplusplus
typedef class DebuggerEvents DebuggerEvents;
#else
typedef struct DebuggerEvents DebuggerEvents;
#endif /* __cplusplus */

#endif 	/* __DebuggerEvents_FWD_DEFINED__ */


void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __VisVim_LIBRARY_DEFINED__
#define __VisVim_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: VisVim
 * at Fri Feb 13 03:01:38 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



DEFINE_GUID(LIBID_VisVim,0xAC726707,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#ifndef __ICommands_INTERFACE_DEFINED__
#define __ICommands_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICommands
 * at Fri Feb 13 03:01:38 1998
 * using MIDL 3.01.75
 ****************************************/
/* [object][dual][oleautomation][uuid] */ 



DEFINE_GUID(IID_ICommands,0xAC726703,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("AC726703-2977-11D1-B2F3-006008040780")
    ICommands : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE VisVimMethod( void) = 0;
        
    };
    
#else 	/* C style interface */

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
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VisVimMethod )( 
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


#define ICommands_VisVimMethod(This)	\
    (This)->lpVtbl -> VisVimMethod(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ICommands_VisVimMethod_Proxy( 
    ICommands __RPC_FAR * This);


void __RPC_STUB ICommands_VisVimMethod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICommands_INTERFACE_DEFINED__ */


#ifdef __cplusplus
DEFINE_GUID(CLSID_Commands,0xAC726704,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

class DECLSPEC_UUID("AC726704-2977-11D1-B2F3-006008040780")
Commands;
#endif

#ifdef __cplusplus
DEFINE_GUID(CLSID_ApplicationEvents,0xAC726705,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

class DECLSPEC_UUID("AC726705-2977-11D1-B2F3-006008040780")
ApplicationEvents;
#endif

#ifdef __cplusplus
DEFINE_GUID(CLSID_DebuggerEvents,0xAC726706,0x2977,0x11D1,0xB2,0xF3,0x00,0x60,0x08,0x04,0x07,0x80);

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
