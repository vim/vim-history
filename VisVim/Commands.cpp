#include "stdafx.h"
#include <comdef.h>	// For _bstr_t
#include "VisVim.h"
#include "Commands.h"
#include "OleAut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif


// Change directory before opening file?
#define CD_SOURCE		0	// cd to source path
#define CD_SOURCE_PARENT	1	// cd to parent directory of source path
#define CD_NONE			2	// no cd


static BOOL g_bEnableVim = TRUE;	// Vim enabled
static BOOL g_bDevStudioEditor = FALSE;	// Open file in Dev Studio editor simultaneously
static int g_ChangeDir = CD_NONE;	// CD after file open?


static COleAutomationControl VimOle;	// OLE automation object for com. with Vim


/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands ()
{
	m_pApplication == NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands ()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release ();
}

void CCommands::SetApplicationObject (IApplication * pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	// for us, which CDSAddIn did in its QueryInterface call
	// just before it called us.
	m_pApplication = pApplication;

	// Create Application event handlers
	XApplicationEventsObj::CreateInstance (&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef ();
	m_pApplicationEventsObj->Connect (m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;

#ifdef NEVER
	// Create Debugger event handler
	CComPtr < IDispatch > pDebugger;
	if (SUCCEEDED (m_pApplication->get_Debugger (&pDebugger))
	    && pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance (&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef ();
		m_pDebuggerEventsObj->Connect (pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}
#endif

	// Get settings from registry HKEY_CURRENT_USER\Software\Vim\VisVim
	HKEY hAppKey = GetAppKey ("Vim");
	if (hAppKey)
	{
		HKEY hSectionKey = GetSectionKey (hAppKey, "VisVim");
		if (hSectionKey)
		{
			g_bEnableVim = GetRegistryInt (hSectionKey, "EnableVim",
						       g_bEnableVim);
			g_bDevStudioEditor = GetRegistryInt(hSectionKey,"DevStudioEditor",
							    g_bDevStudioEditor);
			g_ChangeDir = GetRegistryInt (hSectionKey, "ChangeDir",
						      g_ChangeDir);
			RegCloseKey (hSectionKey);
		}
		RegCloseKey (hAppKey);
	}
}

void CCommands::UnadviseFromEvents ()
{
	// Destroy OLE connection
	VimOle.DeleteObject ();

	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect (m_pApplication);
	m_pApplicationEventsObj->Release ();
	m_pApplicationEventsObj = NULL;

#ifdef NEVER
	if (m_pDebuggerEventsObj)
	{
		// Since we were able to connect to the Debugger events, we
		// should be able to access the Debugger object again to
		// unadvise from its events (thus the VERIFY_OK below--see
		// stdafx.h).
		CComPtr < IDispatch > pDebugger;
		VERIFY_OK (m_pApplication->get_Debugger (&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect (pDebugger);
		m_pDebuggerEventsObj->Release ();
		m_pDebuggerEventsObj = NULL;
	}
#endif
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// Application events

HRESULT CCommands::XApplicationEvents::BeforeBuildStart ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish (long nNumErrors, long nNumWarnings)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

// The open document event handle is the place where the real interface work
// is done.
// Vim gets called from here.
//
HRESULT CCommands::XApplicationEvents::DocumentOpen (IDispatch * theDocument)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());

	if (! g_bEnableVim)
		// Vim not enabled or empty command line entered
		return S_OK;

	// First get the current file name and line number

	CComQIPtr < ITextDocument, &IID_ITextDocument > pDoc (theDocument);
	if (! pDoc)
		return S_OK;

	BSTR FileName;
	long LineNr = -1;
	HRESULT hr;

	hr = pDoc->get_FullName (&FileName);
	if (FAILED (hr))
		return S_OK;

	LPDISPATCH lpdisp;
	hr = pDoc->get_Selection (&lpdisp);
	if (SUCCEEDED (hr))
	{
		CComQIPtr < ITextSelection, &IID_ITextSelection > pSel (lpdisp);
		if (! pSel)
		{
			lpdisp->Release ();
			return S_OK;
		}

		hr = pSel->get_CurrentLine (&LineNr);

		lpdisp->Release ();
	}

	// Initialize Vim OLE connection if not already done
	if (! VimOle.IsCreated ())
	{
		if (! VimOle.CreateObject ("Vim.Application"))
			goto OleError;
	}

	// Get the dispatch id for the SendKeys method.
	// By doing this, we are checking if Vim is still there...
	DISPID DispatchId;
	DispatchId = VimOle.GetDispatchId ("SendKeys");
	if (! DispatchId)
	{
		// We can't get a dispatch id.
		// This means that probably Vim has been terminated.
		// Don't issue an error message here, instead
		// destroy the OLE object and try to connect once more
		VimOle.DeleteObject ();
		if (! VimOle.CreateObject ("Vim.Application"))
			// If this create fails, it's time for an error msg
			goto OleError;

		if (! (DispatchId = VimOle.GetDispatchId ("SendKeys")))
			// There is something wrong...
			goto OleError;
	}

	// Make Vim open the file
	OLECHAR Buf[MAX_OLE_STR];
	char VimCmd[MAX_OLE_STR];
	char* s;

	if (g_ChangeDir != CD_NONE)
	{
		// Do a :cd first

		// Get the path name of the file ("dir/")
		CString StrFileName = FileName;
		char Drive[_MAX_DRIVE];
		char Dir[_MAX_DIR];
		_splitpath (StrFileName, Drive, Dir, NULL, NULL);
		// Convert to unix path name format
		for (char* s = Dir; *s; ++s)
			if (*s == '\\')
				*s = '/';

		// Construct the cd command; append /.. if cd to parent
		// directory and not in root directory
		sprintf (VimCmd, ":cd %s%s%s\n", Drive, Dir,
			 g_ChangeDir == CD_SOURCE_PARENT && Dir[1] ? ".." : "");
		VimOle.Method (DispatchId, "s", TO_OLE_STR_BUF (VimCmd, Buf));
	}

	// Open file
	sprintf (VimCmd, ":e %S\n", (char*) FileName);
	// convert all \ to / 
	for (s = VimCmd; *s; ++s)
		if (*s == '\\')
			*s = '/';
	if (! VimOle.Method (DispatchId, "s", TO_OLE_STR_BUF (VimCmd, Buf)))
		goto OleError;

	if (LineNr > 0)
	{
		// Goto line
		sprintf (VimCmd, ":%d\n", LineNr);
		if (! VimOle.Method (DispatchId, "s", TO_OLE_STR_BUF (VimCmd, Buf)))
			goto OleError;
	}

	// Make Vim come to the foreground
	if (! VimOle.Method ("SetForeground"))
		VimOle.ErrDiag ();

	if (! g_bDevStudioEditor)
	{
		// Close the document in developer studio
		CComVariant vSaveChanges = dsSaveChangesPrompt;
		DsSaveStatus Saved;

		pDoc->Close (vSaveChanges, &Saved);
	}

	// We're done
	return S_OK;

    OleError:
	// There was an OLE error
	// Check if it's the "unknown class string" error
	SCODE sc = GetScode (VimOle.GetResult ());
	if (sc == CO_E_CLASSSTRING)
	{
		char Buf[256];
		sprintf (Buf, "There is no registered OLE automation server named "
			 "\"Vim.Application\".\n"
			 "Use the OLE-enabled version of Vim with VisVim and "
			 "make sure to register Vim by running \"vim -register\".");
		MessageBox (NULL, Buf, "OLE Error", MB_OK);
	}
	else
		VimOle.ErrDiag ();

	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose (IDispatch * theDocument)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave (IDispatch * theDocument)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument (IDispatch * theDocument)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate (IDispatch * theWindow)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate (IDispatch * theWindow)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit (IDispatch * pBreakpoint)
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// VisVim dialog

class CMainDialog : public CDialog
{
    public:
	CMainDialog (CWnd * pParent = NULL);	// standard constructor

	//{{AFX_DATA(CMainDialog)
	enum { IDD = IDD_ADDINMAIN };
	BOOL	m_bEnableVim;
	BOOL	m_bDevStudioEditor;
	int	m_ChangeDir;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CMainDialog)
    protected:
	virtual void DoDataExchange (CDataExchange * pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

    protected:
	//{{AFX_MSG(CMainDialog)
	afx_msg void OnEnable();
	afx_msg void OnDisable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP ()
};

CMainDialog::CMainDialog (CWnd * pParent /* =NULL */ )
	: CDialog (CMainDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainDialog)
	m_bEnableVim = TRUE;
	m_bDevStudioEditor = FALSE;
	m_ChangeDir = -1;
	//}}AFX_DATA_INIT
}

void CMainDialog::DoDataExchange (CDataExchange * pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMainDialog)
	DDX_Check (pDX, IDC_DEVSTUDIO_EDITOR, m_bDevStudioEditor);
	DDX_Radio(pDX, IDC_CD_SOURCE_PATH, m_ChangeDir);
	//}}AFX_DATA_MAP
}

void CMainDialog::OnEnable ()
{
	m_bEnableVim = TRUE;
	UpdateData (true);
	EndDialog (IDOK);
}

void CMainDialog::OnDisable ()
{
	m_bEnableVim = FALSE;
	UpdateData (true);
	EndDialog (IDOK);
}

BEGIN_MESSAGE_MAP (CMainDialog, CDialog)
 //{{AFX_MSG_MAP(CMainDialog)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()


/////////////////////////////////////////////////////////////////////////////
// CCommands methods

STDMETHODIMP CCommands::VisVimMethod ()
{
	AFX_MANAGE_STATE (AfxGetStaticModuleState ());

	// Use m_pApplication to access the Developer Studio Application
	// object,
	// and VERIFY_OK to see error strings in DEBUG builds of your add-in
	// (see stdafx.h)

	VERIFY_OK (m_pApplication->EnableModeless (VARIANT_FALSE));

	CMainDialog Dlg;

	Dlg.m_bEnableVim = g_bEnableVim;
	Dlg.m_bDevStudioEditor = g_bDevStudioEditor;
	Dlg.m_ChangeDir = g_ChangeDir;
	if (Dlg.DoModal () == IDOK)
	{
		g_bEnableVim = Dlg.m_bEnableVim;
		g_bDevStudioEditor = Dlg.m_bDevStudioEditor;
		g_ChangeDir = Dlg.m_ChangeDir;

		// Save settings to registry HKEY_CURRENT_USER\Software\Vim\VisVim
		HKEY hAppKey = GetAppKey ("Vim");
		if (hAppKey)
		{
			HKEY hSectionKey = GetSectionKey (hAppKey, "VisVim");
			if (hSectionKey)
			{
				WriteRegistryInt (hSectionKey, "EnableVim", g_bEnableVim);
				WriteRegistryInt (hSectionKey, "DevStudioEditor",
						  g_bDevStudioEditor);
				WriteRegistryInt (hSectionKey, "ChangeDir", g_ChangeDir);
				RegCloseKey (hSectionKey);
			}
			RegCloseKey (hAppKey);
		}
	}

	VERIFY_OK (m_pApplication->EnableModeless (VARIANT_TRUE));
	return S_OK;
}

