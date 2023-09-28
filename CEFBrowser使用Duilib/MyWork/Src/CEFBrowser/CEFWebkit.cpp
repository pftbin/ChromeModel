#include "stdafx.h"
#include "CEFWebkit.h"

namespace DuiLib
{
	CCEFWebkitUI::CCEFWebkitUI(HWND hParent) :nHitIndex_(0)
	{
		hWebKitBrowserWnd_ = hParent;
		clientHandler_ = new CCEFClientHandler();
		clientHandler_->hWnd_ = hWebKitBrowserWnd_;
	}

	CCEFWebkitUI::~CCEFWebkitUI()
	{

	}

	LPCTSTR CCEFWebkitUI::GetClass() const
	{
		return _T("WKEWebkitUI");
	}
	LPVOID CCEFWebkitUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_WKEWEBKIT) == 0)
		{
			return static_cast<CCEFWebkitUI*>(this);
		}

		return CControlUI::GetInterface(pstrName);
	}

	void CCEFWebkitUI::DoInit()
	{
	}

	void CCEFWebkitUI::SetPos(RECT rc)
	{
		__super::SetPos(rc);

		CefRefPtr<CefBrowser> pBrowser = nullptr;
		CefRefPtr<CefBrowserHost>  pBrowerHost = nullptr;
		CefWindowHandle hwnd = NULL;

		if (clientHandler_ != NULL)
		{

			for (UINT idx = 0; idx < clientHandler_.get()->browser_list_.size(); idx++)
			{

				pBrowser = clientHandler_.get()->browser_list_.at(idx);

				if (pBrowser)
				{
					pBrowerHost = pBrowser->GetHost();

					if (pBrowerHost)
					{

						hwnd = pBrowerHost->GetWindowHandle();
						if ((hwnd != NULL) && (IsWindow(hwnd)))
						{

							if (idx != nHitIndex_)
							{
								//MoveWindow(hwnd, 0, 0, 0, 0, false);
								ShowWindow(hwnd, SW_HIDE);
							}
							else
							{
								ShowWindow(hwnd, SW_SHOW);
								MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);
							}
						}
					}
				}
			}
		}
	}

	void CCEFWebkitUI::DoEvent(TEventUI& event)
	{
		__super::DoEvent(event);
	}

	void CCEFWebkitUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("url")) == 0)
		{
			//	strUrl_ = pstrValue;
		}
		else
		{
			CControlUI::SetAttribute(pstrName, pstrValue);
		}
	}


	//新增页签
	void CCEFWebkitUI::NewPage(CefString Url)
	{
		RECT rectnew = GetPos();
		CefRect rcCefWnd(rectnew.left, rectnew.top, rectnew.right - rectnew.left, rectnew.bottom - rectnew.top);

		CefWindowInfo winInfo;
		winInfo.SetAsChild(hWebKitBrowserWnd_, rcCefWnd);

		CefBrowserSettings browserSettings;
		browserSettings.background_color = CefColorSetARGB(0, 0, 0, 0);

		//将字符串传递到Render进程
		CString strExtraString = _T("custom");
		CefRefPtr<CefDictionaryValue> extra_info = CefDictionaryValue::Create();
		extra_info->SetString("input_extra", static_cast<CefString>(strExtraString));
		CefBrowserHost::CreateBrowser(winInfo, static_cast<CefRefPtr<CefClient>>(clientHandler_), Url.c_str(), browserSettings, extra_info, nullptr);
	}

	//删除页签
	void CCEFWebkitUI::DelPage(int nWebBrowserID)
	{
		int idx = 0;
		BOOL bNeedRemove = FALSE;
		vector<CefRefPtr<CefBrowser>>::const_iterator  itTmpWebBrower = clientHandler_->browser_list_.begin();
		for (; itTmpWebBrower != clientHandler_->browser_list_.end(); itTmpWebBrower++)
		{
			++idx;
			if (nWebBrowserID == itTmpWebBrower->get()->GetIdentifier())
			{
				if (clientHandler_->browser_list_.size() == 1)
				{
					itTmpWebBrower->get()->GetMainFrame()->LoadURL(_T("about:blank"));
				}
				else
				{
					bNeedRemove = TRUE;
					::MoveWindow(itTmpWebBrower->get()->GetHost()->GetWindowHandle(), 0, 0, 0, 0, true);
					nHitIndex_ = idx - 1;
				}
				break;
			}
		}

		if (bNeedRemove)
		{	
			clientHandler_->CloseHostBrowser(itTmpWebBrower->get(), true);
			//clientHandler_->browser_list_.erase(itTmpWebBrower);
		}
		ReFresh();
	}

	void CCEFWebkitUI::CloseAllPage()
	{
		clientHandler_->CloseAllBrowsers(true);
	}


	BOOL CCEFWebkitUI::IsClosed() const
	{
		if (clientHandler_->IsClosing())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void CCEFWebkitUI::ReFresh()
	{

		RECT rectnew = GetPos();
		SetPos(rectnew);
	}

	void CCEFWebkitUI::ReFresh(int nWebBrowserID)
	{
		int idx = 0;
		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{
			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				nHitIndex_ = idx;
			}

			++idx;
		}

		ReFresh();
	}

	void CCEFWebkitUI::ReLoad(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{
			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				it->get()->Reload();
			}
		}
	}

	void CCEFWebkitUI::LoadURL(int nWebBrowserID, CefString & strURL)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{
			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				it->get()->GetMainFrame()->LoadURL(strURL);
			}
		}

		ReFresh(nWebBrowserID);
	}

	CefString CCEFWebkitUI::GetFinalURL(int nWebBrowserID)
	{
		CefString strURL = "";
		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{
			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				strURL = it->get()->GetMainFrame()->GetURL();
			}
		}

		return strURL;
	}

	BOOL CCEFWebkitUI::CanGoForward(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				
				return it->get()->CanGoForward();
			}
		}

		return FALSE;
	}

	void CCEFWebkitUI::GoForward(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{

				if (it->get()->CanGoForward()!=false)
				{
					it->get()->GoForward();
				}

			}
		}

	}


	BOOL CCEFWebkitUI::CanGoBack(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				
				return it->get()->CanGoBack();
			}
		}
		return FALSE;
	}

	void CCEFWebkitUI::GoBack(int nWebBrowserID)
	{
		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				if (it->get()->CanGoBack())
				{
					it->get()->GoBack();
				}
				
			}
		}

	}
	CefRefPtr<CefBrowser> CCEFWebkitUI::GetBrowserByID(int nWebBrowserID)
	{

		for (auto it = clientHandler_->browser_list_.begin(); it != clientHandler_->browser_list_.end(); it++)
		{

			if (nWebBrowserID == it->get()->GetIdentifier())
			{
				return it->get();
			}
		}

		return nullptr;
	}
	
	int CCEFWebkitUI::GetHitIndex() const
	{
		return nHitIndex_;
	}

	void CCEFWebkitUI::SetHitIndex(int idx)
	{
		nHitIndex_ = idx;
		ReFresh();
	}

}