#include "stdafx.h"
#include "BkgWorkCpp.h"
#include "BkgWorkCppDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CBkgWorkCppApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CBkgWorkCppApp::CBkgWorkCppApp()
{
}

CBkgWorkCppApp theApp;

BOOL CBkgWorkCppApp::InitInstance()
{

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("Warning: Application will unexpectedly terminate as the dialog box creation failed"));

	CBkgWorkCppDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: Application will unexpectedly terminate as the dialog box creation failed\n");
		TRACE(traceAppMsg, 0, "Warning: Cannot perform #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS when using MFC controls in dialog boxes\n");
	}

	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	return FALSE;
}
