#pragma once
#include "afxdialogex.h"

#include <cef_app.h>
#include <cef_client.h>

#include "CEFClientHandler.h"
#include "CEFAppHandler.h"

class ChromeModel
{
public:
	ChromeModel() 
	{
		m_bCEFInitState = FALSE;
	};

	~ChromeModel() 
	{
	};

	BOOL InitCEFModel()
	{
		if (m_bCEFInitState==TRUE)
			return TRUE;

		CefMainArgs main_args(::GetModuleHandle(nullptr));

		//	CefRefPtr<CefApp> app;								//默认CefApp
		CefRefPtr<CEFAppHandler> app(new CEFAppHandler());	//继承CefApp支持修改window对象

		CefSettings settings;
		CefSettingsTraits::init(&settings);
		settings.multi_threaded_message_loop = true;
		//	settings.no_sandbox = true;
		CefString(&settings.locale).FromWString(L"UTF-8");
		CefString(&settings.cache_path).FromString("C:/cachepath/");

#if 0
		//主程序App作为Render进程
		int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
		if (exit_code >= 0)
		{
			return FALSE;
		}
#else
		// 获取子进程路径
		TCHAR szPath[MAX_PATH] = {0};
		GetAppFolder(szPath, sizeof szPath);
		lstrcat(szPath, _T("\\ChromeRender.exe"));
		cef_string_from_wide(szPath, MAX_PATH, &settings.browser_subprocess_path);// 设置子进程路径

#endif

		CefInitialize(main_args, settings, app.get(), nullptr);
		m_bCEFInitState = TRUE;

		return TRUE;
	}

	BOOL UnInitCEFModel()
	{
		if (m_bCEFInitState==FALSE)
			return TRUE;

		CefShutdown();
		m_bCEFInitState = FALSE;

		return TRUE;
	}

protected:
	BOOL m_bCEFInitState;
};


// CEFViewDialog 对话框

class CEFViewDialog : public CDialog , public CEFClientHandler::Delegate
{
	DECLARE_DYNAMIC(CEFViewDialog)

public:
	CEFViewDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEFViewDialog();

	void SetHttpURL(CString strInputURL) 
	{
		m_strUrl = strInputURL; 
	}

	void DetachDelegate()
	{
		if (m_clientHandler != nullptr)
		{
			m_clientHandler->DetachDelegate();
		}
	}

// 对话框数据
	enum { IDD = IDD_CHROME_DLG };

public:
	HICON						m_hIcon;
	CMenu						m_menu;
	CToolBar					m_ToolBar;
	CStatusBar					m_statusBar;

protected:
	CefRefPtr<CEFClientHandler> m_clientHandler;
	CefRefPtr<CefBrowser>		m_browser;
	CWnd*						m_wndMain;
	CString						m_strUrl;

protected:
	//Implement Delegate 
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) override;
	virtual void OnSetAddress(std::string const& url) override;
	virtual void OnSetTitle(std::string const& title) override;
	virtual void OnSetFullscreen(bool const fullscreen) override;
	virtual void OnSetLoadingState(bool const isLoading, bool const canGoBack, bool const canGoForward) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void GetCEFViewBorder(CRect& rc);
	void Navigate(CString const& url);
	void NavigateBack();
	void NavigateForward();
	bool CanNavigateBack();
	bool CanNavigateForward();
	void CloseBrowser();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnClose();

	//菜单项
	afx_msg void OnMenuBack();
	afx_msg void OnUpdateMenuBack(CCmdUI* pCmdUI);
	afx_msg void OnMenuForward();
	afx_msg void OnUpdateMenuForward(CCmdUI* pCmdUI);
	afx_msg void OnMenuRefresh();
	afx_msg void OnUpdateMenuRefresh(CCmdUI* pCmdUI);
	afx_msg void OnMenuHome();
	afx_msg void OnUpdateMenuHome(CCmdUI* pCmdUI);

	//工具栏
	afx_msg void OnToolBarButtonClicked(UINT nID);

	//状态栏


	//Resizing实现
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
