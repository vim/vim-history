/* vim: set sw=4 sts=4 : */
/*
 * uvim.c/uvim.cpp
 *
 * "class uvim" to give access to Vim OLE functionality
 *
 * When       Who       What
 * 1999-??-?? Anon      Original
 * 2001-08-04 W.Briscoe Added dual C/C++ capability. Wrote STANDALONE demo.
 */

#ifndef COBJMACROS
# define COBJMACROS /* Enable "friendlier" access to objects */
#endif

#pragma warning(disable: 4100 4115 4201 4214 4514)
#include <windows.h>
#pragma warning(default: 4100 4115 4201 4214)

#include "uvim.h"

/* makes copy of passed string -- new string allocated using SysAllocString() */
    static BSTR
DuplicateAsSysBstr(const char *sStr)
{
    int nLen = (int)strlen(sStr);
    int nCnt = MultiByteToWideChar(CP_ACP, 0, sStr, nLen, NULL, 0);
    BSTR bstrRet = SysAllocStringLen(NULL, (unsigned)(nCnt + 1));

    MultiByteToWideChar(CP_ACP, 0, sStr, nLen, bstrRet, nCnt);
    bstrRet[nCnt] = 0;
    return bstrRet;
}

/* like function above, but in reverse direction */
    static char *
DuplicateAsStr(const BSTR bstr)
{
    int nLen = WideCharToMultiByte(CP_ACP, 0, bstr, -1, NULL, 0, NULL, NULL);
#ifdef __cplusplus
    char *sRet = new char[(size_t)nLen];
#else
    char *sRet = malloc((size_t)nLen);

    if (sRet)
#endif
	WideCharToMultiByte(CP_ACP, 0, bstr, -1, sRet, nLen, NULL, NULL);
    return sRet;
}

#ifdef __cplusplus
    void
uvim::SendKeys(const char *sKeys)
{
#if 0
    /* IVim interface is not implemented */
    if (m_pDisp != NULL)
    {
	BSTR bstrVal = DuplicateAsSysBstr(sKeys);

	m_pDisp->SendKeys(bstrVal);
	SysFreeString(bstrVal);
    }
#else
    OLECHAR *szMember = L"SendKeys";
    DISPID dispid;
    VARIANTARG varg;
    DISPPARAMS params = {NULL, NULL, 1, 0};

    params.rgvarg = &varg;

    if (m_pDisp != NULL
	    && !FAILED(m_pDisp->GetIDsOfNames(IID_NULL, &szMember, 1,
		    LOCALE_SYSTEM_DEFAULT, &dispid)))
    {
	varg.vt = VT_BSTR;
	varg.bstrVal = DuplicateAsSysBstr(sKeys);
	m_pDisp->Invoke(dispid, IID_NULL,
				       LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
		&params, NULL, NULL, NULL);
	SysFreeString(varg.bstrVal);
    }
#endif
}
#else
    static void
SendKeys(uvim *this, const char *sKeys)
{
    OLECHAR *szMember = (OLECHAR*)L"SendKeys";
    DISPID dispid;
    VARIANTARG varg;
    DISPPARAMS params = {NULL, NULL, 1, 0};

    params.rgvarg = &varg;

    if (this->m_pDisp != NULL
	    && !FAILED(IDispatch_GetIDsOfNames(this->m_pDisp,
		    &IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
    {
	varg.vt = VT_BSTR;
	varg.bstrVal = DuplicateAsSysBstr(sKeys);
	IDispatch_Invoke(this->m_pDisp, dispid, &IID_NULL,
		LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD, &params, NULL, NULL, NULL);
	SysFreeString(varg.bstrVal);
    }
#if 0
    /* IVim interface is not implemented */
    uvim *m_pVim = ((uvim*)o)->x;
    if (m_pVim != NULL)
    {
	BSTR bstrVal = DuplicateAsSysBstr(sKeys);

	IVim_SendKeys(m_pVim, bstrVal);
	SysFreeString(bstrVal);
    }
#endif
}
#endif

#ifdef __cplusplus
    char *
uvim::Eval(const char *sExpr)
{
    OLECHAR *szMember = L"Eval";
    DISPID dispid;
    VARIANTARG varg;
    VARIANTARG vret;
    char *sRet = NULL;
    DISPPARAMS params = {NULL, NULL, 1, 0};

    params.rgvarg = &varg;

    if (m_pDisp != NULL
	    && !FAILED(m_pDisp->GetIDsOfNames(IID_NULL, &szMember, 1,
		    LOCALE_SYSTEM_DEFAULT, &dispid)))
    {
	varg.vt = VT_BSTR;
	varg.bstrVal = DuplicateAsSysBstr(sExpr);
	VariantInit(&vret);

	HRESULT res = m_pDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD, &params, &vret, NULL, NULL);

	SysFreeString(varg.bstrVal);

	if (SUCCEEDED(res))
	{
	    sRet = DuplicateAsStr(vret.bstrVal);
	    SysFreeString(vret.bstrVal);
	    return sRet;
	}
    }

    return NULL;
}
#else
    static char *
Eval(uvim *this, const char *sExpr)
{
    OLECHAR *szMember = (OLECHAR*)L"Eval";
    DISPID dispid;
    VARIANTARG varg;
    VARIANTARG vret;
    char *sRet = NULL;
    DISPPARAMS params = {NULL, NULL, 1, 0};

    params.rgvarg = &varg;

    if (this->m_pDisp != NULL
	    && !FAILED(IDispatch_GetIDsOfNames(this->m_pDisp,
		    &IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
    {
	HRESULT res;

	varg.vt = VT_BSTR;
	varg.bstrVal = DuplicateAsSysBstr(sExpr);
	VariantInit(&vret);

	res = IDispatch_Invoke(this->m_pDisp, dispid, &IID_NULL,
		LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD,
						  &params, &vret, NULL, NULL);

	SysFreeString(varg.bstrVal);

	if (SUCCEEDED(res))
	{
	    sRet = DuplicateAsStr(vret.bstrVal);
	    SysFreeString(vret.bstrVal);
	    return sRet;
	}
    }

    return NULL;
}
#endif

#ifdef __cplusplus
    unsigned
uvim::GetHwnd()
{
    OLECHAR *szMember = L"GetHwnd";
    DISPID dispid;
    VARIANTARG vret = {0};
    DISPPARAMS params = {NULL, NULL, 0, 0};

    if (m_pDisp == NULL
	    ||  FAILED(m_pDisp->GetIDsOfNames(IID_NULL, &szMember, 1,
		    LOCALE_SYSTEM_DEFAULT, &dispid))
	    ||  FAILED(m_pDisp->Invoke(dispid, IID_NULL,
		    LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, &vret,
								  NULL, NULL))
	    ||  vret.vt != VT_UI4)
	return 0;
    return (unsigned)vret.lVal;
}
#else
    static unsigned
GetHwnd(uvim *this)
{
    OLECHAR *szMember = (OLECHAR*)L"GetHwnd";
    DISPID dispid;
    VARIANTARG vret = {0};
    DISPPARAMS params = {NULL, NULL, 0, 0};

    if (this->m_pDisp == NULL
	    ||  FAILED(IDispatch_GetIDsOfNames(this->m_pDisp, &IID_NULL,
								 &szMember, 1,
		    LOCALE_SYSTEM_DEFAULT, &dispid))
	    ||  FAILED(IDispatch_Invoke(this->m_pDisp, dispid, &IID_NULL,
		    LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, &vret,
								  NULL, NULL))
	    ||  vret.vt != VT_UI4)
	return 0;
    return (unsigned)vret.lVal;
}
#endif

#ifdef __cplusplus
    void
uvim::SetForeground()
{
    OLECHAR *szMember = L"SetForeground";
    DISPID dispid;
    DISPPARAMS params = {NULL, NULL, 0, 0};

    if (m_pDisp != NULL && !FAILED(m_pDisp->GetIDsOfNames(IID_NULL,
								 &szMember, 1,
		    LOCALE_SYSTEM_DEFAULT, &dispid)))
	m_pDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
							      DISPATCH_METHOD,
		&params, NULL, NULL, NULL);
}
#else
    static void
SetForeground(uvim *this)
{
    OLECHAR *szMember = (OLECHAR*)L"SetForeground";
    DISPID dispid;
    DISPPARAMS params = {NULL, NULL, 0, 0};

    if (this->m_pDisp != NULL && !FAILED(IDispatch_GetIDsOfNames(this->m_pDisp,
		    &IID_NULL, &szMember, 1, LOCALE_SYSTEM_DEFAULT, &dispid)))
	IDispatch_Invoke(this->m_pDisp, dispid, &IID_NULL,
							LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD, &params, NULL, NULL, NULL);
}
#endif

#ifdef __cplusplus
uvim::uvim()
{
#if 0
    // {0F0BFAE1-4C90-11D1-82D7-0004AC368519}
    static const CLSID m_CLSID_Vim = {0x0f0bfae1, 0x4c90, 0x11d1,
	0x82, 0xd7, 0x00, 0x04, 0xac, 0x36, 0x85, 0x19};
#else
    CLSID m_CLSID_Vim;
#endif

    m_pDisp = NULL;
    if (FAILED(CoInitialize(NULL))
#if 0
#else
	    ||  FAILED(CLSIDFromProgID(L"Vim.Application", &m_CLSID_Vim))
#endif
	    ||  FAILED(CoCreateInstance(m_CLSID_Vim, NULL, CLSCTX_LOCAL_SERVER,
		    IID_IDispatch, (void**)&m_pDisp))) {
	(void)MessageBox(NULL, "Failed to obtain vim interface.\n"
			 "Please run vim to register it !", "vim_ole",
			 MB_OK | MB_ICONEXCLAMATION);
    }
}
#else
    uvim
touvim(void)
{
#if 0
    // {0F0BFAE1-4C90-11D1-82D7-0004AC368519}
    static const CLSID m_CLSID_Vim = {0x0f0bfae1, 0x4c90, 0x11d1,
	0x82, 0xd7, 0x00, 0x04, 0xac, 0x36, 0x85, 0x19};
#else
    CLSID m_CLSID_Vim;
#endif
    uvim ret = {NULL, SendKeys, Eval, SetForeground, GetHwnd};

    if (FAILED(CoInitialize(NULL))
#if 0
#else
	    ||  FAILED(CLSIDFromProgID((OLECHAR*)L"Vim.Application",
								&m_CLSID_Vim))
#endif
	    ||  FAILED(CoCreateInstance(&m_CLSID_Vim, NULL, CLSCTX_LOCAL_SERVER,
		    &IID_IDispatch, (void**)&ret.m_pDisp))) {
	(void)MessageBox(NULL, "Failed to obtain vim interface.\n"
			 "Please run vim to register it !", "vim_ole",
			 MB_OK | MB_ICONEXCLAMATION);
    }

    return ret;
}
#endif

#ifdef __cplusplus
uvim::~uvim()
{
    if (m_pDisp != NULL)
	m_pDisp->Release();

    CoUninitialize();
}
#else
    void
unuvim(uvim *this)
{
    if (this->m_pDisp != NULL)
	IDispatch_Release(this->m_pDisp);

    CoUninitialize();
}
#endif

#ifdef STANDALONE

#include <stdio.h>

int main(void)
{
    char *twelve;
    const char *six_plus_six = "6 + 6";
    unsigned window;

#ifdef __cplusplus
    uvim vim;

    twelve = vim.Eval(six_plus_six);
    window = vim.GetHwnd();
    vim.SendKeys(":e c:/autoexec.bat\n");
#else
    uvim vim = touvim();

    twelve = vim.Eval(&vim, six_plus_six);
    window = vim.GetHwnd(&vim);
    vim.SendKeys(&vim, ":e c:/autoexec.bat\n");
    unuvim(&vim);
#endif

    printf("vim.Eval(\"%s\") is \"%s\"\n", six_plus_six, twelve);
    printf("vim.GetHwnd() is %u\n", window);
    return 0;
}

#endif
