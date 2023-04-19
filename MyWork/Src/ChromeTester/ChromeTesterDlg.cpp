
// ChromeTesterDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChromeTester.h"
#include "ChromeTesterDlg.h"
#include "afxdialogex.h"

#define  DF_TIMER_CENTER_CEFWND			11
#define	 WM_DESTORY_CHROME	(WM_USER + 1000)

extern BOOL GetAppFolder(LPTSTR szFolder, int nBufSize);
extern void Char2CString(char* pBuffer, CString& pWideString);

extern BOOL	 g_bCEFInitState;
extern BOOL  (*g_pInitCefModel)();
extern BOOL  (*g_pUnInitCefModel)();
extern BOOL  (*g_pSetWindowParam)(const CString& strUrl, const CString& strTemplateXml, const CString& strMosId, const CString& strRoomName,BOOL& bLocalList);
extern BOOL  (*g_pCreateChromeWindow)(CWnd* pParentWnd);
extern CWnd* (*g_pGetChromeWindow)();
extern void  (*g_pDestroyChromeWindow)();

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CChromeTesterDlg 对话框

CChromeTesterDlg::CChromeTesterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHROMETESTER_DIALOG, pParent)
	, m_strInputUrl(_T("www.baidu.com"))
	, m_strInputTemplateXml("")
	, m_strInputMosId("sobey.StudioD.mos")
	, m_strInputRoomName("数字孪生演播室-高清")
	, m_bLocalList(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pChromeWnd = NULL;
}

CChromeTesterDlg::~CChromeTesterDlg()
{
	if (m_pChromeWnd && g_pDestroyChromeWindow)
	{
		g_pDestroyChromeWindow();
		m_pChromeWnd = NULL;
	}
}

void CChromeTesterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_URL, m_strInputUrl);
}

LRESULT CChromeTesterDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CLOSE)
	{
		if (m_pChromeWnd && g_pDestroyChromeWindow)
		{
			g_pDestroyChromeWindow();
			m_pChromeWnd = NULL;
		}

		if (g_bCEFInitState)
		{
//			g_pUnInitCefModel();
//			g_bCEFInitState = FALSE;
		}
	}

	return __super::WindowProc(message, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CChromeTesterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_GO, &CChromeTesterDlg::OnBnClickedButtonGo)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChromeTesterDlg 消息处理程序

BOOL CChromeTesterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChromeTesterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChromeTesterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChromeTesterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChromeTesterDlg::OnBnClickedButtonGo()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_strInputUrl.IsEmpty())
	{
		AfxMessageBox(_T("请输入网址..."));
		return;
	}

	//
	TCHAR szXMLPath[MAX_PATH];
	GetAppFolder(szXMLPath, sizeof szXMLPath);
	lstrcat(szXMLPath, _T("\\TemplateXml.txt"));
	CFile file;
	if (file.Open(szXMLPath, CFile::modeRead))
	{
		UINT uiReadOffset = 0;
		UINT uiReadSize = 1024 * 1024;
	
		file.Seek(uiReadOffset, CFile::begin);
		char* pBuffer = new char[uiReadSize + 1];
		memset(pBuffer, 0, sizeof(char) * (uiReadSize + 1));
		file.Read(pBuffer, uiReadSize);
		pBuffer[uiReadSize] = 0;
		file.Close();

		CString strContent;
		Char2CString(pBuffer, strContent);
		m_strInputTemplateXml = strContent;
		if (pBuffer)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}
	else
	{
		AfxMessageBox(_T("未找到XML文件 [TemplateXml.txt]"));
		return;
	}

	//
	if (g_bCEFInitState && g_pCreateChromeWindow && g_pGetChromeWindow)
	{
		if (m_pChromeWnd->GetSafeHwnd())
			g_pDestroyChromeWindow();
		m_pChromeWnd = NULL;

		BOOL bResult = TRUE;
		bResult &= g_pSetWindowParam(m_strInputUrl, m_strInputTemplateXml, m_strInputMosId, m_strInputRoomName,m_bLocalList);
		if (bResult)
			bResult &= g_pCreateChromeWindow(this);
		if (bResult)
			m_pChromeWnd = g_pGetChromeWindow();
		
		if (m_pChromeWnd)
		{
			m_pChromeWnd->ShowWindow(SW_SHOW);
			SetTimer(DF_TIMER_CENTER_CEFWND, 1000, NULL);//网页加载会先白屏,需要延时居中显示
		}
		else
			AfxMessageBox(_T("创建窗口失败..."));
	}
}

void CChromeTesterDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == DF_TIMER_CENTER_CEFWND)
	{
		if (m_pChromeWnd && m_pChromeWnd->GetSafeHwnd())
		{
			m_pChromeWnd->CenterWindow();
		}
		KillTimer(DF_TIMER_CENTER_CEFWND);
	}
}

void CChromeTesterDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnClose();
}
