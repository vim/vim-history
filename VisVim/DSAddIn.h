// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__AC726715_2977_11D1_B2F3_006008040780__INCLUDED_)
#define AFX_DSADDIN_H__AC726715_2977_11D1_B2F3_006008040780__INCLUDED_

#include "commands.h"

//{AC726702-2977-11D1-B2F3-006008040780}
DEFINE_GUID (CLSID_DSAddIn,
	0xac726702, 0x2977, 0x11d1, 0xb2, 0xf3, 0, 0x60, 0x8, 0x4, 0x7, 0x80);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn :
	public   IDSAddIn,
	public   CComObjectRoot,
	public   CComCoClass < CDSAddIn,
		 &CLSID_DSAddIn >
{
    public:
	DECLARE_REGISTRY (CDSAddIn, "VisVim.DSAddIn.1",
			  "VisVim Developer Studio Add-in", IDS_VISVIM_LONGNAME,
			  THREADFLAGS_BOTH)

	CDSAddIn ()
	{
	}

	BEGIN_COM_MAP (CDSAddIn)
	COM_INTERFACE_ENTRY (IDSAddIn)
	END_COM_MAP ()
	DECLARE_NOT_AGGREGATABLE (CDSAddIn)

	// IDSAddIns
    public:
	STDMETHOD (OnConnection) (THIS_ IApplication * pApp, VARIANT_BOOL bFirstTime,
				  long dwCookie, VARIANT_BOOL * OnConnection);
	STDMETHOD (OnDisconnection) (THIS_ VARIANT_BOOL bLastTime);

    protected:
	bool AddCommand (IApplication* pApp, char* MethodName, char* CmdName,
			 UINT StrResId, UINT GlyphIndex, VARIANT_BOOL bFirstTime);

    protected:
	CCommandsObj * m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DSADDIN_H__AC726715_2977_11D1_B2F3_006008040780__INCLUDED)
