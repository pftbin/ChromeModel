#pragma once
#include "afxdialogex.h"
#include "SkinDlg.h"

#include <cef_app.h>
#include <cef_client.h>
#include "CWebClient.h"
#include "CWebProcessApp.h"

#include "AirMsgBox.h"
#include "JSONXMLHelper.h"
#include "ChromeModelDefine.h"


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
		CefRefPtr<CWebProcessApp> app(new CWebProcessApp());	//继承CefApp支持修改window对象

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


// ChromeUIDlg 对话框

class ChromeUIDlg : public CSkinDlg
{
	DECLARE_DYNAMIC(ChromeUIDlg)

public:
	ChromeUIDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ChromeUIDlg();

// 对话框数据
	enum { IDD = IDD_CHROME_DLG };

public:
	HICON					m_hIcon;

	CBitmap					m_hBitmapCloseUp;
	CBitmap					m_hBitmapCloseHover;
	CBitmap					m_hBitmapCloseDown;
	CBitmap					m_hBitmapMax1;
	CBitmap					m_hBitmapRestore1;
	CBitmap					m_hBitmapTitleBarL;
	CBitmap					m_hBitmapTitleBarC;
	CBitmap					m_hBitmapTitleBarR;
	CBitmap					m_hBitmapDialogBk;


	CString					m_strUrl;
	CString					m_strTemplateXml;
	CString					m_strMosId;
	CString					m_strRoomName;
	BOOL					m_bLocalList;
	BOOL					m_bDataNotify;

public:
	BOOL GetReqSaveData(CString& strHead, CString& strBody);
	BOOL CanCloseWnd();

protected:
	CefRefPtr<CWebClient>	m_cWebClient;
	CString					m_strPostReqSaveHead;
	CString					m_strPostReqSaveBody;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	BOOL MessageBoxAir(HWND hWnd, CString strMsg, CString strTitle, UINT uid);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestory();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	//Resizing实现
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//
	afx_msg LRESULT OnPostRequestSave(WPARAM wParam, LPARAM lParam);
};
