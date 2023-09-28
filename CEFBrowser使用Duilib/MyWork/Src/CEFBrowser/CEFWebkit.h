#pragma  once 
#include "stdafx.h"
#include <vector>
#include "CEFClientHandler.h"

namespace DuiLib
{
	class CCEFWebkitUI : public CControlUI
	{
	public:
		CCEFWebkitUI(HWND hParent);
		~CCEFWebkitUI();

		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void DoInit();
		virtual void DoEvent(TEventUI& event);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetPos(RECT rc);

	public:
		void NewPage(CefString Url);
		void DelPage(int nWebBrowserID);
		void CloseAllPage();
		BOOL IsClosed() const;
		void ReFresh();
		void ReFresh(int nWebBrowserID);
		void ReLoad(int nWebBrowserID);
		void LoadURL(int nWebBrowserID, CefString& strURL);
		CefString GetFinalURL(int nWebBrowserID);

		BOOL CanGoForward(int nWebBrowserID);
		void GoForward(int nWebBrowserID);
		BOOL CanGoBack(int nWebBrowserID);
		void GoBack(int nWebBrowserID);
		CefRefPtr<CefBrowser> GetBrowserByID(int nWebBrowserID);

		int GetHitIndex() const;
		void SetHitIndex(int idx);

	protected:

		HWND hWebKitBrowserWnd_;
		//std::vector<std::wstring> strURLs_;
		CefRefPtr<CCEFClientHandler>  clientHandler_; //CefClient实现，用于处理Browser实例相关的回调
		int nHitIndex_;
	};
};
