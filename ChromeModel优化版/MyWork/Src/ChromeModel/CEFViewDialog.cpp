// CEFViewDialog.cpp: 实现文件
//

#include "pch.h"
#include "Resource.h"
#include "ChromeModel.h"
#include "CEFViewDialog.h"
#include "afxdialogex.h"

#define DF_TIMER_STATE_TIME		10
static UINT indicators[] =	//添加状态栏：状态、地址、时间
{
	IDS_STRING_STATE,
	IDS_STRING_NOWURL,
	IDS_STRING_TIME
};


// CEFViewDialog 对话框

IMPLEMENT_DYNAMIC(CEFViewDialog, CDialog)

CEFViewDialog::CEFViewDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CHROME_DLG, pParent)
{
	m_strUrl = _T("http://www.baidu.com");
}

CEFViewDialog::~CEFViewDialog()
{
}

void CEFViewDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

BOOL CEFViewDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	ModifyStyle(0, WS_SYSMENU | WS_CAPTION);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	int x = ::GetSystemMetrics(SM_CXSCREEN);
	int y = ::GetSystemMetrics(SM_CYSCREEN);
	x = max(x, 512); y = max(y, 256);
	CRect rcWindow(x / 8, y / 8, 3 * x / 4, 3 * y / 4);
	MoveWindow(rcWindow, TRUE);

	CRect rcClient; 
	GetClientRect(&rcClient);

	// 创建菜单
	m_menu.LoadMenu(IDR_MENU_MAIN);
	SetMenu(&m_menu);

	// 创建工具栏
	m_ToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_ToolBar.LoadToolBar(IDR_TOOLBAR_MAIN);
	CRect rcToolBar;
	m_ToolBar.GetToolBarCtrl().SetButtonSize(CSize(32, 32));
	m_ToolBar.GetToolBarCtrl().GetWindowRect(&rcToolBar);
	ScreenToClient(&rcToolBar);
	rcToolBar.OffsetRect(0, 0);
	m_ToolBar.MoveWindow(&rcToolBar);
	m_ToolBar.SetOwner(this);

	// 创建状态栏、调整布局
	m_statusBar.Create(this);                   //创造状态栏
	m_statusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));   //设置状态栏项目栏数
	m_statusBar.SetPaneInfo(0, IDS_STRING_STATE, SBPS_NORMAL, 100);
	m_statusBar.SetPaneInfo(1, IDS_STRING_NOWURL, SBPS_STRETCH, 0);
	m_statusBar.SetPaneInfo(2, IDS_STRING_TIME, SBPS_NORMAL, 150);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);//指定状态栏在显示窗口位置
	SetTimer(DF_TIMER_STATE_TIME, 800, NULL);

	CRect rcCEFView;
	GetCEFViewBorder(rcCEFView);
	CefRect rcCefWnd(rcCEFView.left, rcCEFView.top, rcCEFView.Width(), rcCEFView.Height());

	CefWindowInfo winInfo;
	winInfo.SetAsChild(GetSafeHwnd(), rcCefWnd);

	CefBrowserSettings browserSettings;
	browserSettings.background_color = CefColorSetARGB(255, 255, 255, 0);
	//browserSettings.web_security = STATE_DISABLED;

	CefRefPtr<CEFClientHandler> client(new CEFClientHandler(this));//继承CefClient支持嵌入
	m_clientHandler = client;

	//将字符串传递到Render进程
	CString strExtraString = _T("custom");
	CefRefPtr<CefDictionaryValue> extra_info = CefDictionaryValue::Create();
	extra_info->SetString("input_extra", static_cast<CefString>(strExtraString));
	CefBrowserHost::CreateBrowser(winInfo, static_cast<CefRefPtr<CefClient>>(client), static_cast<CefString>(m_strUrl), browserSettings, extra_info, nullptr);

	return TRUE;
}

BOOL CEFViewDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_F5)
		{
			m_browser->Reload();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

////////////////////////////////////////////
void Toggle(HWND const root_hwnd, UINT const nCmdShow)
{
	// Retrieve current window placement information.
	WINDOWPLACEMENT placement;
	::GetWindowPlacement(root_hwnd, &placement);

	if (placement.showCmd == nCmdShow)
		::ShowWindow(root_hwnd, SW_RESTORE);
	else
		::ShowWindow(root_hwnd, nCmdShow);
}

HWND GetRootHwnd(CefRefPtr<CefBrowser> browser)
{
	return ::GetAncestor(browser->GetHost()->GetWindowHandle(), GA_ROOT);
}

void Minimize(CefRefPtr<CefBrowser> browser)
{
	Toggle(GetRootHwnd(browser), SW_MINIMIZE);
}

void Maximize(CefRefPtr<CefBrowser> browser)
{
	Toggle(GetRootHwnd(browser), SW_MAXIMIZE);
}

void Restore(CefRefPtr<CefBrowser> browser)
{
	::ShowWindow(GetRootHwnd(browser), SW_RESTORE);
}

BEGIN_MESSAGE_MAP(CEFViewDialog, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_COMMAND(ID_CEF_BACK, &CEFViewDialog::OnMenuBack)
	ON_UPDATE_COMMAND_UI(ID_CEF_BACK, &CEFViewDialog::OnUpdateMenuBack)
	ON_COMMAND(ID_CEF_FORWARD, &CEFViewDialog::OnMenuForward)
	ON_UPDATE_COMMAND_UI(ID_CEF_FORWARD, &CEFViewDialog::OnUpdateMenuForward)
	ON_COMMAND(ID_CEF_REFRESH, &CEFViewDialog::OnMenuRefresh)
	ON_UPDATE_COMMAND_UI(ID_CEF_REFRESH, &CEFViewDialog::OnUpdateMenuRefresh)
	ON_COMMAND(ID_CEF_HOME, &CEFViewDialog::OnMenuHome)
	ON_UPDATE_COMMAND_UI(ID_CEF_HOME, &CEFViewDialog::OnUpdateMenuHome)
	ON_COMMAND_RANGE(ID_CEF_BACK, ID_CEF_HOME, OnToolBarButtonClicked)
END_MESSAGE_MAP()

////////////////////////////////////////////
void CEFViewDialog::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
	m_browser = browser;
}

void CEFViewDialog::OnBrowserClosing(CefRefPtr<CefBrowser> browser)
{
}

void CEFViewDialog::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
	if (m_browser != nullptr && m_browser->GetIdentifier() == browser->GetIdentifier())
	{
		m_browser = nullptr;
		m_clientHandler->DetachDelegate();
	}
}

void CEFViewDialog::OnSetAddress(std::string const& url)
{
	m_strUrl = url.c_str();
}

void CEFViewDialog::OnSetTitle(std::string const& title)
{
	::SetWindowText(m_hWnd, CefString(title).ToWString().c_str());
}

void CEFViewDialog::OnSetFullscreen(bool const fullscreen)
{
	if (m_browser != nullptr)
	{
		if (fullscreen)
		{
			Maximize(m_browser);
		}
		else
		{
			Restore(m_browser);
		}
	}
}

void CEFViewDialog::OnSetLoadingState(bool const isLoading, bool const canGoBack, bool const canGoForward)
{
}

// CEFViewDialog 消息处理程序
////////////////////////////////////////////
void CEFViewDialog::GetCEFViewBorder(CRect& rc)
{
	GetClientRect(&rc);
	//rc.top += 40;	//菜单栏
	rc.bottom -= 40;//状态栏要占用客户区
	rc.left += 10;
	rc.right -= 10;
}

void CEFViewDialog::Navigate(CString const& url)
{
	if (m_browser != nullptr)
	{
		auto frame = m_browser->GetMainFrame();
		if (frame != nullptr)
		{
			frame->LoadURL(CefString(url));
		}
	}
}

void CEFViewDialog::NavigateBack()
{
	if (m_browser != nullptr)
	{
		m_browser->GoBack();
	}
}

void CEFViewDialog::NavigateForward()
{
	if (m_browser != nullptr)
	{
		m_browser->GoForward();
	}
}

bool CEFViewDialog::CanNavigateBack()
{
	return m_browser != nullptr && m_browser->CanGoBack();
}

bool CEFViewDialog::CanNavigateForward()
{
	return m_browser != nullptr && m_browser->CanGoForward();
}

void CEFViewDialog::CloseBrowser()
{
	if (m_browser != nullptr)
	{
		::DestroyWindow(m_browser->GetHost()->GetWindowHandle());
		//m_browser->GetHost()->CloseBrowser(true);
	}
}

void CEFViewDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == DF_TIMER_STATE_TIME)
	{
		SYSTEMTIME st; CString strTime;
		GetLocalTime(&st);
		strTime.Format(_T("%02d:%02d:%02d"), st.wHour, st.wMinute, st.wSecond);
		m_statusBar.SetPaneText(1, m_strUrl);
		m_statusBar.SetPaneText(2, strTime);
	}

	__super::OnTimer(nIDEvent);
}

afx_msg BOOL CEFViewDialog::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
}

afx_msg void CEFViewDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//TODO: 在此处添加消息处理程序代码
	if (m_clientHandler != nullptr)
	{
		if (m_browser != nullptr)
		{
			CRect rcCEFView;
			GetCEFViewBorder(rcCEFView);
			CefWindowHandle hwnd = m_browser->GetHost()->GetWindowHandle();
			::SetWindowPos(hwnd, HWND_TOP, rcCEFView.left, rcCEFView.top, rcCEFView.Width(), rcCEFView.Height(), SWP_NOZORDER);
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		}
	}
}

afx_msg void CEFViewDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//调整最小高度与宽度
	lpMMI->ptMinTrackSize.x = 800;
	lpMMI->ptMinTrackSize.y = 600;

	//调整最大高度与宽度
	int width = ::GetSystemMetrics(SM_CXSCREEN);
	int height = ::GetSystemMetrics(SM_CYSCREEN);
	lpMMI->ptMaxTrackSize.x = width;
	lpMMI->ptMaxTrackSize.y = height;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

afx_msg void CEFViewDialog::OnClose()
{
	CloseBrowser();

	CDialog::OnClose();
}

//菜单项
afx_msg void CEFViewDialog::OnMenuBack()
{
	NavigateBack();
}
afx_msg void CEFViewDialog::OnUpdateMenuBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_hWnd != nullptr && CanNavigateBack() ? TRUE : FALSE);
}
afx_msg void CEFViewDialog::OnMenuForward()
{
	NavigateForward();
}
afx_msg void CEFViewDialog::OnUpdateMenuForward(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_hWnd != nullptr && CanNavigateForward() ? TRUE : FALSE);
}
afx_msg void CEFViewDialog::OnMenuRefresh()
{
	Navigate(m_strUrl);
}
afx_msg void CEFViewDialog::OnUpdateMenuRefresh(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_hWnd != nullptr ? TRUE : FALSE);
}
afx_msg void CEFViewDialog::OnMenuHome()
{
	Navigate(_T("www.baidu.com"));
}
afx_msg void CEFViewDialog::OnUpdateMenuHome(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_hWnd != nullptr ? TRUE: FALSE);
}

//工具栏
afx_msg void CEFViewDialog::OnToolBarButtonClicked(UINT nID)
{
	switch (nID)
	{
		case ID_CEF_BACK:
			OnMenuBack();
			break;
		case ID_CEF_FORWARD:
			OnMenuForward();
			break;
		case ID_CEF_REFRESH:
			OnMenuRefresh();
			break;
		case ID_CEF_HOME:
			OnMenuHome();
			break;
	}
}

//状态栏


//Resizing实现
afx_msg LRESULT CEFViewDialog::OnNcHitTest(CPoint point)
{
	CRect       rcWindow;
	GetWindowRect(&rcWindow);
	if (point.x <= rcWindow.left + 3)
	{
		return HTLEFT;
	}
	else if (point.x >= rcWindow.right - 3)
	{
		return HTRIGHT;
	}
	else if (point.y <= rcWindow.top + 3)
	{
		return HTTOP;
	}
	else if (point.y >= rcWindow.bottom - 3)
	{
		return HTBOTTOM;
	}
	else if (
		(point.x <= rcWindow.left + 10)
		&& (point.y <= rcWindow.top + 10)
		)
	{
		return HTTOPLEFT;
	}
	else if (
		(point.x >= rcWindow.right - 10)
		&& (point.y <= rcWindow.top + 10)
		)
	{
		return HTTOPRIGHT;
	}
	else if (
		(point.x <= rcWindow.left + 10)
		&& (point.y >= rcWindow.bottom - 10)
		)
	{
		return HTBOTTOMLEFT;
	}
	else if (
		(point.x >= rcWindow.right - 10)
		&& (point.y >= rcWindow.bottom - 10)
		)
	{
		return HTBOTTOMRIGHT;
	}
	else
	{
		return CDialog::OnNcHitTest(point);
	}
}
afx_msg BOOL CEFViewDialog::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTTOP || nHitTest == HTBOTTOM)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
		return TRUE;
	}
	else if (nHitTest == HTLEFT || nHitTest == HTRIGHT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
		return TRUE;
	}
	else if (nHitTest == HTTOPLEFT || nHitTest == HTBOTTOMRIGHT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENWSE)));
		return TRUE;
	}
	else if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMLEFT)
	{
		SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENESW)));
		return TRUE;
	}
	else
	{
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	}
}

