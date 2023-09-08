// ChromeUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChromeModel.h"
#include "ChromeUIDlg.h"
#include "afxdialogex.h"

// ChromeUIDlg 对话框

IMPLEMENT_DYNAMIC(ChromeUIDlg, CSkinDlg)

ChromeUIDlg::ChromeUIDlg(CWnd* pParent /*=nullptr*/)
	: CSkinDlg(IDD_CHROME_DLG, pParent)
{
	m_strUrl = _T("http://www.baidu.com");
	m_strTemplateXml = _T("XML init in Dialog");
	m_strMosId = _T("MosId init in Dialog");
	m_strRoomName = _T("RoomName init in Dialog");
	m_bDataNotify = FALSE;

	m_strPostReqSaveHead = _T("");
	m_strPostReqSaveBody = _T("");
}

ChromeUIDlg::~ChromeUIDlg()
{
}

BOOL ChromeUIDlg::GetReqSaveData(CString& strHead, CString& strBody)
{
	if (m_strPostReqSaveHead.IsEmpty() || m_strPostReqSaveBody.IsEmpty())
		return FALSE;

	strHead = m_strPostReqSaveHead;
	strBody = m_strPostReqSaveBody;

	return TRUE;
}

void ChromeUIDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

BOOL ChromeUIDlg::OnInitDialog()
{
	CSkinDlg::OnInitDialog();
	ModifyStyle(WS_CAPTION | WS_THICKFRAME, WS_MAXIMIZEBOX);


	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//int x = ::GetSystemMetrics(SM_CXSCREEN);
	//int y = ::GetSystemMetrics(SM_CYSCREEN);
	//x = max(x, 512); y = max(y, 256);
	//CRect rcDlg(x/8, y-10, 3*x/4, 3*y/4); //宽高3/4,居中隐藏到下方(网页加载会先白屏,加载完成再居中显示)
	//MoveWindow(rcDlg, TRUE);
	::SendMessage(m_hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

	//皮肤
	m_hBitmapCloseUp.LoadBitmap(IDB_BITMAP_UP);
	m_hBitmapCloseHover.LoadBitmap(IDB_BITMAP_HOVER);
	m_hBitmapCloseDown.LoadBitmap(IDB_BITMAP_DOWN);
	HBITMAP hBmpUp = (HBITMAP)m_hBitmapCloseUp.m_hObject;
	HBITMAP hBmpHover = (HBITMAP)m_hBitmapCloseHover.m_hObject;
	HBITMAP hBmpDown = (HBITMAP)m_hBitmapCloseDown.m_hObject;
	m_tBar.SetCloseBtnBmp(hBmpUp, hBmpHover, hBmpDown);
	
	m_hBitmapMax1.LoadBitmap(IDB_BITMAP_MAX1);
	m_hBitmapRestore1.LoadBitmap(IDB_BITMAP_RESTORE1);
	HBITMAP hBmpMax1 = (HBITMAP)m_hBitmapMax1.m_hObject;
	HBITMAP hBmpRestore1 = (HBITMAP)m_hBitmapRestore1.m_hObject;
	m_tBar.SetShowMax(TRUE);
	m_tBar.SetMaxBtnUpBmp(hBmpMax1, hBmpMax1, hBmpMax1);
	m_tBar.SetResBtnUpBmp(hBmpRestore1, hBmpRestore1, hBmpRestore1);
	m_tBar.SetShowMin(FALSE);

	m_hBitmapTitleBarL.LoadBitmap(IDB_BITMAP_TBL);
	m_hBitmapTitleBarC.LoadBitmap(IDB_BITMAP_TBC);
	m_hBitmapTitleBarR.LoadBitmap(IDB_BITMAP_TBR);
	HBITMAP hBmpBarBkL = (HBITMAP)m_hBitmapTitleBarL.m_hObject;
	HBITMAP hBmpBarBkC = (HBITMAP)m_hBitmapTitleBarC.m_hObject;
	HBITMAP hBmpBarBkR = (HBITMAP)m_hBitmapTitleBarR.m_hObject;
	m_tBar.SetBackGroundBmp(hBmpBarBkL, hBmpBarBkC, hBmpBarBkR);
	m_tBar.SetTitle((_T("")), RGB(255, 255, 255));

	m_hBitmapDialogBk.LoadBitmap(IDB_BITMAP_DLGBK);
	HBITMAP hBmpDialogBk = (HBITMAP)m_hBitmapDialogBk.m_hObject;
	SetBkgndBmp(hBmpDialogBk);
	SetBkgndColor(RGB(0, 0, 0));
	//

	CRect rc;
	GetClientRect(rc);
	rc.DeflateRect(10, 10, 10, 10);
	CefRect rcCefWnd(rc.left, rc.top, rc.Width(), rc.Height());

	CefRefPtr<CWebClient>  client(new CWebClient(this));//继承CefClient支持嵌入
	client->SetCustomData(m_bLocalList);
	m_cWebClient = client;

	CefWindowInfo winInfo;
	winInfo.SetAsChild(GetSafeHwnd(), rcCefWnd);
	CefBrowserSettings browserSettings;
	browserSettings.background_color = CefColorSetARGB(255, 0, 0, 0);

	CefRefPtr<CefDictionaryValue> extra_info = CefDictionaryValue::Create();
	extra_info->SetString("info_TemplateXml", static_cast<CefString>(m_strTemplateXml));
	extra_info->SetString("info_MosId", static_cast<CefString>(m_strMosId));
	extra_info->SetString("info_RoomName", static_cast<CefString>(m_strRoomName));
	CefBrowserHost::CreateBrowser(winInfo, static_cast<CefRefPtr<CefClient>>(client), static_cast<CefString>(m_strUrl), browserSettings, extra_info,nullptr);

	return TRUE;
}

BEGIN_MESSAGE_MAP(ChromeUIDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_POSTREQUEST_SAVE, &ChromeUIDlg::OnPostRequestSave)
END_MESSAGE_MAP()


// ChromeUIDlg 消息处理程序
afx_msg BOOL ChromeUIDlg::OnEraseBkgnd(CDC* pDC)
{
	return CSkinDlg::OnEraseBkgnd(pDC);
}

afx_msg void ChromeUIDlg::OnSize(UINT nType, int cx, int cy)
{
	CSkinDlg::OnSize(nType, cx, cy);

	//TODO: 在此处添加消息处理程序代码
	if (m_cWebClient.get())
	{
		CefRefPtr<CefBrowser> browser = m_cWebClient->GetBrowser();
		if (browser.get())
		{
			CRect rc;
			GetClientRect(&rc);
			rc.DeflateRect(10, 10, 10, 10);
			CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
			::MoveWindow(hwnd, rc.left, rc.top, rc.Width(), rc.Height(), TRUE);
		}
	}
}

afx_msg void ChromeUIDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_cWebClient.get())
	{
		CefRefPtr<CefBrowser> browser = m_cWebClient->GetBrowser();
		if (browser.get())
		{
			// Notify the browser window that we would like to close it. This
			// will result in a call to ClientHandler::DoClose() if the
			// JavaScript 'onbeforeunload' event handler allows it.
			browser->StopLoad();
			browser->GetHost()->CloseBrowser(false);
		}
	}

	//此窗口从外部动态创建，通知外部主动销毁(或外部打开新窗口时主动销毁旧窗口)
#if 0
	CWnd* pParentWnd = GetParent();
	if (pParentWnd && pParentWnd->GetSafeHwnd())
	{
		::PostMessage(pParentWnd->GetSafeHwnd(), WM_DESTORY_CHROME, 0, 0);
	}
#endif

	CSkinDlg::OnClose();

	DestroyWindow();
}

afx_msg void ChromeUIDlg::OnDestory()
{
	if (m_cWebClient.get())
	{
		CefRefPtr<CefBrowser> browser = m_cWebClient->GetBrowser();
		if (browser.get())
		{
			// Notify the browser window that we would like to close it. This
			// will result in a call to ClientHandler::DoClose() if the
			// JavaScript 'onbeforeunload' event handler allows it.
			browser->StopLoad();
			browser->GetHost()->CloseBrowser(false);
		}
	}

	CSkinDlg::OnDestroy();
}

afx_msg void ChromeUIDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
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

	CSkinDlg::OnGetMinMaxInfo(lpMMI);
}

//
afx_msg LRESULT ChromeUIDlg::OnNcHitTest(CPoint point)
{
	CRect       rectWindow;
	GetWindowRect(&rectWindow);
	if (point.x <= rectWindow.left + 3)
	{
		return HTLEFT;
	}
	else if (point.x >= rectWindow.right - 3)
	{
		return HTRIGHT;
	}
	else if (point.y <= rectWindow.top + 3)
	{
		return HTTOP;
	}
	else if (point.y >= rectWindow.bottom - 3)
	{
		return HTBOTTOM;
	}
	else if (
		(point.x <= rectWindow.left + 10)
		&& (point.y <= rectWindow.top + 10)
		)
	{
		return HTTOPLEFT;
	}
	else if (
		(point.x >= rectWindow.right - 10)
		&& (point.y <= rectWindow.top + 10)
		)
	{
		return HTTOPRIGHT;
	}
	else if (
		(point.x <= rectWindow.left + 10)
		&& (point.y >= rectWindow.bottom - 10)
		)
	{
		return HTBOTTOMLEFT;
	}
	else if (
		(point.x >= rectWindow.right - 10)
		&& (point.y >= rectWindow.bottom - 10)
		)
	{
		return HTBOTTOMRIGHT;
	}
	else
	{
		return CDialog::OnNcHitTest(point);
	}
}

afx_msg BOOL ChromeUIDlg::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
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

//
afx_msg LRESULT ChromeUIDlg::OnPostRequestSave(WPARAM wParam, LPARAM lParam)
{
	//BOOL bLocalList = (BOOL)wParam;
	//if(bLocalList)
	//	AfxMessageBox(_T("POST请求消息=local")); 
	//else
	//	AfxMessageBox(_T("POST请求消息=normal"));
	//return 0;

	if (m_cWebClient.get())
	{
		m_cWebClient->GetPostReqSaveData(m_strPostReqSaveHead, m_strPostReqSaveBody);

		//CFile REQFile(_T("C:\\REQData.txt"), CFile::modeCreate | CFile::modeWrite);
		//int nLen0 = 0;
		//char* pBuffer0 = CStringTopchar(m_strPostReqSaveBody, nLen0);
		//REQFile.Write(pBuffer0, sizeof(char) * nLen0);
		//REQFile.Flush();
		//REQFile.Close();

		//Body 是Json格式，转换为XML格式
		m_strPostReqSaveBody.Replace(_T("\\\""), _T("#"));	// \" => #
		CString strXMLBody = Json2XmlEx(m_strPostReqSaveBody);

		strXMLBody.Replace(_T("\\\""), _T("#")); // \" => #
		strXMLBody.Replace(_T("\""), _T(""));	// clear "
		strXMLBody.Replace(_T("#"), _T("\""));  // #  => "

		//CFile GETFile(_T("C:\\XMLData.txt"), CFile::modeCreate | CFile::modeWrite);
		//int nLen = 0;
		//char* pBuffer = CStringTopchar(strXMLBody, nLen);
		//GETFile.Write(pBuffer, sizeof(char) * nLen);
		//GETFile.Flush();
		//GETFile.Close();

		m_strPostReqSaveBody = strXMLBody;
		m_bDataNotify = TRUE;//外部获取此值，知晓数据是否有更新
	}

	return 0;
}

