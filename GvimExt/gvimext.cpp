/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	gvimext by Tianmiao Hu
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * gvimext is a DLL which is used for the "Edit with Vim" context menu
 * extension.  It implements a MS defined interface with the Shell.
 *
 * If you have any questions or any suggestions concerning gvimext, please
 * contact Tianmiao Hu: tianmiao@acm.org.
 */

#include "gvimext.h"

// Always get an error while putting the following stuff to the
// gvimext.h file as class protected variables, give up and
// declare them as global stuff
FORMATETC fmte = {CF_HDROP,
		  (DVTARGETDEVICE FAR *)NULL,
		  DVASPECT_CONTENT,
		  -1,
		  TYMED_HGLOBAL
		 };
STGMEDIUM medium;
HRESULT hres = 0;
UINT cbFiles = 0;

//
// Global variables
//
UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL;	// Handle to this DLL itself.

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
	// Extension DLL one-time initialization
	g_hmodThisDll = hInstance;
    }

    return 1;   // ok
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI DllCanUnloadNow(void)
{
    return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut)
{
    *ppvOut = NULL;

    if (IsEqualIID(rclsid, CLSID_ShellExtension))
    {
	CShellExtClassFactory *pcf = new CShellExtClassFactory;

	return pcf->QueryInterface(riid, ppvOut);
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}

CShellExtClassFactory::CShellExtClassFactory()
{
    m_cRef = 0L;

    g_cRefThisDll++;
}

CShellExtClassFactory::~CShellExtClassFactory()
{
    g_cRefThisDll--;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid,
						   LPVOID FAR *ppv)
{
    *ppv = NULL;

    // Any interface on this object is the object pointer

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
	*ppv = (LPCLASSFACTORY)this;

	AddRef();

	return NOERROR;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
    if (--m_cRef)
	return m_cRef;

    delete this;

    return 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
						      REFIID riid,
						      LPVOID *ppvObj)
{
    *ppvObj = NULL;

    // Shell extensions typically don't support aggregation (inheritance)

    if (pUnkOuter)
	return CLASS_E_NOAGGREGATION;

    // Create the main shell extension object.  The shell will then call
    // QueryInterface with IID_IShellExtInit--this is how shell extensions are
    // initialized.

    LPCSHELLEXT pShellExt = new CShellExt();  //Create the CShellExt object

    if (NULL == pShellExt)
	return E_OUTOFMEMORY;

    return pShellExt->QueryInterface(riid, ppvObj);
}


STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
    return NOERROR;
}

// *********************** CShellExt *************************
CShellExt::CShellExt()
{
    m_cRef = 0L;
    m_pDataObj = NULL;

    g_cRefThisDll++;
}

CShellExt::~CShellExt()
{
    if (m_pDataObj)
	m_pDataObj->Release();

    g_cRefThisDll--;
}

STDMETHODIMP CShellExt::QueryInterface(REFIID riid, LPVOID FAR *ppv)
{
    *ppv = NULL;

    if (IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown))
    {
	*ppv = (LPSHELLEXTINIT)this;
    }
    else if (IsEqualIID(riid, IID_IContextMenu))
    {
	*ppv = (LPCONTEXTMENU)this;
    }

    if (*ppv)
    {
	AddRef();

	return NOERROR;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CShellExt::AddRef()
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExt::Release()
{

    if (--m_cRef)
	return m_cRef;

    delete this;

    return 0L;
}


//
//  FUNCTION: CShellExt::Initialize(LPCITEMIDLIST, LPDATAOBJECT, HKEY)
//
//  PURPOSE: Called by the shell when initializing a context menu or property
//	     sheet extension.
//
//  PARAMETERS:
//    pIDFolder - Specifies the parent folder
//    pDataObj  - Spefifies the set of items selected in that folder.
//    hRegKey   - Specifies the type of the focused item in the selection.
//
//  RETURN VALUE:
//
//    NOERROR in all cases.
//
//  COMMENTS:   Note that at the time this function is called, we don't know
//		(or care) what type of shell extension is being initialized.
//		It could be a context menu or a property sheet.
//

STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder,
				   LPDATAOBJECT pDataObj,
				   HKEY hRegKey)
{
    // Initialize can be called more than once
	HRESULT hres = 0;

    if (m_pDataObj)
	m_pDataObj->Release();

    // duplicate the object pointer and registry handle

    if (pDataObj)
    {
	m_pDataObj = pDataObj;
	pDataObj->AddRef();
    }

    return NOERROR;
}


//
//  FUNCTION: CShellExt::QueryContextMenu(HMENU, UINT, UINT, UINT, UINT)
//
//  PURPOSE: Called by the shell just before the context menu is displayed.
//	     This is where you add your specific menu items.
//
//  PARAMETERS:
//    hMenu      - Handle to the context menu
//    indexMenu  - Index of where to begin inserting menu items
//    idCmdFirst - Lowest value for new menu ID's
//    idCmtLast  - Highest value for new menu ID's
//    uFlags     - Specifies the context of the menu event
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu,
					 UINT indexMenu,
					 UINT idCmdFirst,
					 UINT idCmdLast,
					 UINT uFlags)
{
    UINT idCmd = idCmdFirst;
    BOOL bAppendItems=TRUE;

    hres = m_pDataObj->GetData(&fmte, &medium);
    if (medium.hGlobal)
	cbFiles = DragQueryFile((HDROP)medium.hGlobal, (UINT)-1, 0, 0);

    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Initialize m_cntOfHWnd to 0
    m_cntOfHWnd = 0;
    // Retieve all the vim instances
    EnumWindows(EnumWindowsProc, (LPARAM)this);

    if (cbFiles > 1)
    {
	InsertMenu(hMenu,
		indexMenu++,
		MF_STRING|MF_BYPOSITION,
		idCmd++,
		"Edit with &multiple Vims");

	InsertMenu(hMenu,
		indexMenu++,
		MF_STRING|MF_BYPOSITION,
		idCmd++,
		"Edit with single &Vim");

	// set flag
	m_multiFiles = TRUE;

    }
    else
    {
	InsertMenu(hMenu,
		indexMenu++,
		MF_STRING|MF_BYPOSITION,
		idCmd++,
		"Edit with &Vim");

	// set flag
	m_multiFiles = FALSE;
    }

    // Now display all the vim instances
    for (int i = 0; i < m_cntOfHWnd; i++)
    {
	char title[MAX_PATH];
	char temp[MAX_PATH];

	// Obtain window title, continue if can not
	if (GetWindowText(m_hWnd[i], title, MAX_PATH - 1) == 0)
	    continue;
	// Truncate the title before the path, keep the file name
	char *pos = strchr(title, '(');
	if (pos != NULL)
	{
	    if (pos > title && pos[-1] == ' ')
		--pos;
	    *pos = 0;
	}
	// Now concatenate
	strncpy(temp, "Edit with existing Vim - &", MAX_PATH - 1);
	strncat(temp, title, MAX_PATH - 1);
	InsertMenu(hMenu,
		indexMenu++,
		MF_STRING|MF_BYPOSITION,
		idCmd++,
		temp);
    }
    // InsertMenu(hMenu, indexMenu++, MF_SEPARATOR|MF_BYPOSITION, 0, NULL);

    // Must return number of menu items we added.
    return ResultFromShort(idCmd-idCmdFirst);
}

//
//  FUNCTION: CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO)
//
//  PURPOSE: Called by the shell after the user has selected on of the
//	     menu items that was added in QueryContextMenu().
//
//  PARAMETERS:
//    lpcmi - Pointer to an CMINVOKECOMMANDINFO structure
//
//  RETURN VALUE:
//
//
//  COMMENTS:
//

STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
    HRESULT hr = E_INVALIDARG;

    // If HIWORD(lpcmi->lpVerb) then we have been called programmatically
    // and lpVerb is a command that should be invoked.  Otherwise, the shell
    // has called us, and LOWORD(lpcmi->lpVerb) is the menu ID the user has
    // selected.  Actually, it's (menu ID - idCmdFirst) from QueryContextMenu().
    if (!HIWORD(lpcmi->lpVerb))
    {
	UINT idCmd = LOWORD(lpcmi->lpVerb);

	if (m_multiFiles == TRUE)
	{
	    switch (idCmd)
	    {
		case 0:
		    hr = InvokeGvim(lpcmi->hwnd,
			    lpcmi->lpDirectory,
			    lpcmi->lpVerb,
			    lpcmi->lpParameters,
			    lpcmi->nShow);
		    break;
		case 1:
		    hr = InvokeSingleGvim(lpcmi->hwnd,
			    lpcmi->lpDirectory,
			    lpcmi->lpVerb,
			    lpcmi->lpParameters,
			    lpcmi->nShow);
		    break;
		default:
		    // Existing vim instance
		    hr = PushToWindow(lpcmi->hwnd,
			    lpcmi->lpDirectory,
			    lpcmi->lpVerb,
			    lpcmi->lpParameters,
			    lpcmi->nShow,
			    idCmd - 2);
		    break;
	    }
	}
	else{
	    switch (idCmd)
	    {
		case 0:
		    hr = InvokeGvim(lpcmi->hwnd,
			    lpcmi->lpDirectory,
			    lpcmi->lpVerb,
			    lpcmi->lpParameters,
			    lpcmi->nShow);
		    break;
		default:
		    // Existing vim instance
		    hr = PushToWindow(lpcmi->hwnd,
			    lpcmi->lpDirectory,
			    lpcmi->lpVerb,
			    lpcmi->lpParameters,
			    lpcmi->nShow,
			    idCmd - 1);
		    break;
	    }
	}
    }
    return hr;
}

STDMETHODIMP CShellExt::PushToWindow(HWND hParent,
				   LPCSTR pszWorkingDir,
				   LPCSTR pszCmd,
				   LPCSTR pszParam,
				   int iShowCmd,
				   int idHWnd)
{
    HWND hWnd = m_hWnd[idHWnd];

    // Show and bring vim instance to foreground
    if (IsIconic(hWnd) != 0)
	ShowWindow(hWnd, SW_RESTORE);
    else
	ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);

    // Post the selected files to the vim instance
    PostMessage(hWnd, WM_DROPFILES, (WPARAM)medium.hGlobal, NULL);

    return NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT idCmd,
					 UINT uFlags,
					 UINT FAR *reserved,
					 LPSTR pszName,
					 UINT cchMax)
{
    if (uFlags == GCS_HELPTEXT && cchMax > 35)
	lstrcpy(pszName, "Edits the selected file(s) with Vim");

    return NOERROR;
}

BOOL CALLBACK CShellExt::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    char temp[MAX_PATH];

    // First do a bunch of check
    // No invisible window
    if (!IsWindowVisible(hWnd)) return true;
    // No child window ???
    // if (GetParent(hWnd)) return true;
    // Class name should be Vim, if failed to get class name, return
    if (GetClassName(hWnd, temp, sizeof(temp)) == 0)
	return true;
    // Compare class name to that of vim, if not, return
    if (_strnicmp( temp, "vim", sizeof("vim")) != 0)
	return true;
    // First check if the number of vim instance exceeds MAX_HWND
    CShellExt *cs = (CShellExt*) lParam;
    if (cs->m_cntOfHWnd >= MAX_HWND) return true;
    // Now we get the vim window, put it into some kind of array
    cs->m_hWnd[cs->m_cntOfHWnd] = hWnd;
    cs->m_cntOfHWnd ++;

    return true; // continue enumeration (otherwise this would be false)
}

#ifdef WIN32
/* This symbol is not defined in older versions of the SDK or Visual C++ */

#ifndef VER_PLATFORM_WIN32_WINDOWS
# define VER_PLATFORM_WIN32_WINDOWS 1
#endif

static DWORD g_PlatformId;

/*
 * Set g_PlatformId to VER_PLATFORM_WIN32_NT (NT) or
 * VER_PLATFORM_WIN32_WINDOWS (Win95).
 */
    static void
PlatformId(void)
{
    static int done = FALSE;

    if (!done)
    {
	OSVERSIONINFO ovi;

	ovi.dwOSVersionInfoSize = sizeof(ovi);
	GetVersionEx(&ovi);

	g_PlatformId = ovi.dwPlatformId;
	done = TRUE;
    }
}

# ifndef __BORLANDC__
    static char *
searchpath(char *name)
{
    static char widename[2 * MAX_PATH];
    static char location[2 * MAX_PATH + 2];

    /* There appears to be a bug in FindExecutableA() on Windows NT.
     * Use FindExecutableW() instead... */
    PlatformId();
    if (g_PlatformId == VER_PLATFORM_WIN32_NT)
    {
	MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)name, -1,
		(LPWSTR)widename, MAX_PATH);
	if (FindExecutableW((LPCWSTR)widename, (LPCWSTR)"",
		    (LPWSTR)location) > (HINSTANCE)32)
	{
	    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)location, -1,
		    (LPSTR)widename, 2 * MAX_PATH, NULL, NULL);
	    return widename;
	}
    }
    else
    {
	if (FindExecutableA((LPCTSTR)name, (LPCTSTR)"",
		    (LPTSTR)location) > (HINSTANCE)32)
	    return location;
    }
    return "";
}
# endif
#endif

static void
getGvimName(char *name)
{
    HKEY keyhandle;
    DWORD hlen;

    /* Get the location of gvim from the registry. */
    name[0] = 0;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Vim\\Gvim", 0,
				       KEY_READ, &keyhandle) == ERROR_SUCCESS)
    {
	hlen = MAX_PATH;
	if (RegQueryValueEx(keyhandle, "path", 0, NULL, (BYTE *)name, &hlen)
							     != ERROR_SUCCESS)
	    name[0] = 0;
	else
	    name[hlen] = 0;
	RegCloseKey(keyhandle);
    }
    if (name[0] == 0)
	strcpy(name, searchpath("gvim.exe"));
    if (name[0] == 0)
	strcpy(name, searchpath("gvim.bat"));
    if (name[0] == 0)
	strcpy(name, "gvim");		/* finds gvim.bat or gvim.exe */
}

STDMETHODIMP CShellExt::InvokeGvim(HWND hParent,
				   LPCSTR pszWorkingDir,
				   LPCSTR pszCmd,
				   LPCSTR pszParam,
				   int iShowCmd)
{
    char m_szFileUserClickedOn[MAX_PATH];
    char cmdStr[MAX_PATH];
    UINT i;

    for (i = 0; i < cbFiles; i++)
    {
	DragQueryFile((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		sizeof(m_szFileUserClickedOn));

	getGvimName(cmdStr);
	strcat(cmdStr, " \"");

	if ((strlen(cmdStr) + strlen(m_szFileUserClickedOn) + 2) < MAX_PATH)
	{
	    strcat(cmdStr, m_szFileUserClickedOn);
	    strcat(cmdStr, "\"");

	    STARTUPINFO si;
	    PROCESS_INFORMATION pi;

	    ZeroMemory(&si, sizeof(si));
	    si.cb = sizeof(si);

	    // Start the child process.
	    if (!CreateProcess(NULL,	// No module name (use command line).
			cmdStr,		// Command line.
			NULL,		// Process handle not inheritable.
			NULL,		// Thread handle not inheritable.
			FALSE,		// Set handle inheritance to FALSE.
			0,		// No creation flags.
			NULL,		// Use parent's environment block.
			NULL,		// Use parent's starting directory.
			&si,		// Pointer to STARTUPINFO structure.
			&pi)		// Pointer to PROCESS_INFORMATION structure.
	       )
	    {
		MessageBox(hParent, "Error creating process: Check if gvim is in your path!", "gvimext.dll error", MB_OK);
	    }
            else
            {
                CloseHandle( pi.hProcess );
                CloseHandle( pi.hThread );
            }
	}
	else
	{
	    MessageBox(hParent, "Path length too long!", "gvimext.dll error", MB_OK);
	}
    }

    return NOERROR;
}


STDMETHODIMP CShellExt::InvokeSingleGvim(HWND hParent,
				   LPCSTR pszWorkingDir,
				   LPCSTR pszCmd,
				   LPCSTR pszParam,
				   int iShowCmd)
{
    char m_szFileUserClickedOn[MAX_PATH];
    char cmdStr[MAX_PATH];
    UINT i;

    getGvimName(cmdStr);
    for (i = 0; i < cbFiles; i++)
    {
	DragQueryFile((HDROP)medium.hGlobal,
		i,
		m_szFileUserClickedOn,
		sizeof(m_szFileUserClickedOn));

	if ((strlen(cmdStr) + strlen(m_szFileUserClickedOn) + 4) < MAX_PATH)
	{
	    strcat(cmdStr, " \"");
	    strcat(cmdStr, m_szFileUserClickedOn);
	    strcat(cmdStr, "\"");
	}
	else
	{
	    MessageBox(hParent, "Path length too long or too many files selected!", "gvimext.dll error", MB_OK);

	    return NOERROR;
	}
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Start the child process.
    if (!CreateProcess(NULL,	// No module name (use command line).
		cmdStr,		// Command line.
		NULL,		// Process handle not inheritable.
		NULL,		// Thread handle not inheritable.
		FALSE,		// Set handle inheritance to FALSE.
		0,		// No creation flags.
		NULL,		// Use parent's environment block.
		NULL,		// Use parent's starting directory.
		&si,		// Pointer to STARTUPINFO structure.
		&pi)		// Pointer to PROCESS_INFORMATION structure.
       )
    {
	MessageBox(hParent, "Error creating process: Check if gvim is in your path!", "gvimext.dll error", MB_OK);
    }
    else
    {
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
    }

    return NOERROR;
}

