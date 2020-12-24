

#include "stdafx.h"
#include "BkgWorkCpp.h"
#include "BkgWorkCppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum
	{
		IDD = IDD_ABOUTBOX
	};

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CBkgWorkCppDlg::CBkgWorkCppDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CBkgWorkCppDlg::IDD, pParent), m_dwDelay(200), m_bClosing(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBkgWorkCppDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_DELAY, m_dwDelay);
}

BEGIN_MESSAGE_MAP(CBkgWorkCppDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BTN_START, &CBkgWorkCppDlg::OnBnClickedBtnStart)
ON_BN_CLICKED(IDC_BTN_PAUSE, &CBkgWorkCppDlg::OnBnClickedBtnPause)
ON_BN_CLICKED(IDC_BTN_CANCEL, &CBkgWorkCppDlg::OnBnClickedBtnCancel)
ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CBkgWorkCppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_hPause = CreateEvent(NULL, TRUE, TRUE, NULL);

	m_bw.Initialize(GetSafeHwnd(), Handler_DoWork);
	m_bw.SetReportHandler(Handler_Report);
	m_bw.SetCompletedHandler(Handler_Completed);

	GetPrgBar()->SetRange(0, 100);

	return TRUE;
}

void CBkgWorkCppDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_bw.Uninitialize();
	if (m_hPause != NULL)
		CloseHandle(m_hPause);
}

void CBkgWorkCppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CBkgWorkCppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CBkgWorkCppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void WINAPI CBkgWorkCppDlg::Handler_DoWork(BkgrdWorker *pBW, DoWorkArgs *e)
{
	DWORD dwThrId = GetCurrentThreadId();
	CBkgWorkCppDlg *pDlg = (CBkgWorkCppDlg *)e->Argument;

	pBW->ReportProgress(0, (PVOID)dwThrId);

	for (int i = 1; i <= 100; i++)
	{
		if (pBW->CancellationPending)
		{
			e->Cancel = true;
			break;
		}

		Sleep(pDlg->m_dwDelay);
		pBW->ReportProgress(i);
		if (i == 100)
			break;
		WaitForSingleObject(pDlg->m_hPause, INFINITE);
	}
}

void WINAPI CBkgWorkCppDlg::Handler_Report(BkgrdWorker *pBW, ProgressChangedArgs *e)
{
	DWORD dwThrId = GetCurrentThreadId();
	CBkgWorkCppDlg *pDlg = (CBkgWorkCppDlg *)CWnd::FromHandle(pBW->Owner());

	CEdit *pLog = pDlg->GetLogEdit();
	int nLen = pLog->GetWindowTextLength();
	CString szLog;

	pDlg->GetPrgBar()->SetPos(e->ProgressPercentage);
	if (e->ProgressPercentage == 0)
	{
		szLog.Format(_T("DoWork, Id = %d\xd\xa\xd\xa"), (DWORD)e->UserState);
		pLog->SetSel(nLen, nLen);
		pLog->ReplaceSel(szLog);
	}
	else
	{
		szLog.Format(_T("ProgressChanged => %d%%, Id = %d\xd\xa"), e->ProgressPercentage, dwThrId);

		CString szCur;
		pLog->GetWindowText(szCur);
		szCur = szCur.Left(szCur.GetLength() - 2);
		int nIdx = szCur.ReverseFind(_T('\xd'));
		if (nIdx >= 0)
			pLog->SetSel(nIdx + 2, nLen);
		else
			pLog->SetSel(nLen, nLen);
		pLog->ReplaceSel(szLog);
	}
}

void WINAPI CBkgWorkCppDlg::Handler_Completed(BkgrdWorker *pBW, RunWorkerCompletedArgs *e)
{
	DWORD dwThrId = GetCurrentThreadId();
	CBkgWorkCppDlg *pDlg = (CBkgWorkCppDlg *)CWnd::FromHandle(pBW->Owner());

	if (pDlg->m_bClosing)
	{
		pDlg->DestroyWindow();
		return;
	}

	CEdit *pLog = pDlg->GetLogEdit();
	int nLen = pLog->GetWindowTextLength();
	CString szLog;

	szLog.Format(_T("RunWorker%s, Id = %d\xd\xa"),
				 (e->Cancelled) ? _T("Canceled") : _T("Completed"), dwThrId);
	pLog->SetSel(nLen, nLen);
	pLog->ReplaceSel(szLog);

	pDlg->GetDlgItem(IDC_BTN_START)->EnableWindow();
	pDlg->GetDlgItem(IDC_BTN_PAUSE)->EnableWindow(FALSE);
	pDlg->GetDlgItem(IDC_BTN_CANCEL)->EnableWindow(FALSE);
}

void CBkgWorkCppDlg::OnBnClickedBtnStart()
{
	UpdateData();

	DWORD dwThrId = GetCurrentThreadId();
	CString szLog;
	szLog.Format(_T("RunWorkerAsync, Id = %d\xd\xa"), dwThrId);
	SetDlgItemText(IDC_EDIT_LOG, szLog);

	m_bw.RunWorkerAsync(this);

	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PAUSE)->EnableWindow();
	GetDlgItem(IDC_BTN_CANCEL)->EnableWindow();
}

void CBkgWorkCppDlg::OnBnClickedBtnPause()
{
	if (!m_bw.IsBusy)
		return;

	DWORD dwThrId = GetCurrentThreadId();
	CEdit *pLog = GetLogEdit();
	int nLen = pLog->GetWindowTextLength();
	CString szLog;

	if (WaitForSingleObject(m_hPause, 0) == WAIT_OBJECT_0)
	{
		GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(_T("Restart"));
		ResetEvent(m_hPause);
		szLog.Format(_T(" ** Paused, Id = %d\xd\xa\xd\xa"), dwThrId);
	}
	else
	{
		GetDlgItem(IDC_BTN_PAUSE)->SetWindowText(_T("Pause"));
		SetEvent(m_hPause);
		szLog.Format(_T(" ** Resumed, Id = %d\xd\xa\xd\xa"), dwThrId);
	}
	pLog->SetSel(nLen, nLen);
	pLog->ReplaceSel(szLog);
}

void CBkgWorkCppDlg::OnBnClickedBtnCancel()
{
	if (!m_bw.IsBusy)
		return;

	if (WaitForSingleObject(m_hPause, 0) == WAIT_OBJECT_0)
		ResetEvent(m_hPause);

	int dr = AfxMessageBox(_T("Confirm Cancellation of Current Operation?"), MB_YESNO);
	if (dr == IDYES)
		m_bw.CancelAsync();
	SetEvent(m_hPause);
}

void CBkgWorkCppDlg::OnCancel()
{
	if (!m_bw.IsBusy)
	{
		CDialogEx::OnCancel();
		return;
	}

	if (WaitForSingleObject(m_hPause, 0) == WAIT_OBJECT_0)
		ResetEvent(m_hPause);

	int dr = AfxMessageBox(_T("Confirm Cancellation of Current Operation?"), MB_YESNO);
	if (dr == IDYES)
	{
		m_bClosing = true;
		m_bw.CancelAsync();
	}
	SetEvent(m_hPause);
}
