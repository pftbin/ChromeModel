#pragma once
#include <cef_v8.h>
#include <cef_client.h>
#include <cef_parser.h>
#include "ChromeModelDefine.h"

class CWebClient
	: public CefClient
	, public CefLifeSpanHandler
	, public CefDisplayHandler
	, public CefLoadHandler
	, public CefRequestHandler
	, public CefResourceRequestHandler

{
protected:
	int						m_BrowserId;
	CWnd*					m_pParent;
	BOOL					m_bLocalList;

	CefRefPtr<CefBrowser>	m_Browser;
	CString					m_strHead;
	CString					m_strBody;

public:
	CWebClient(CWnd* pParent = NULL);
	virtual ~CWebClient(void);

	//CustomData
	void SetCustomData(BOOL bLocalList)
	{
		m_bLocalList = bLocalList;
	}

	//CefClient
	CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }
	void GetPostReqSaveData(CString& strHead, CString& strBody)
	{
		strHead = m_strHead;
		strBody = m_strBody;
	}

#if 1
	//CefLifeSpanHandler
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler()
	{
		return this;
	}

	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser)
	{
		m_Browser = browser;
		m_BrowserId = browser->GetIdentifier();
	}

	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access)
	{

		//点击新链接，在当前窗口打开
		switch (target_disposition)
		{
		case WOD_NEW_FOREGROUND_TAB:
		case WOD_NEW_BACKGROUND_TAB:
		case WOD_NEW_POPUP:
		case WOD_NEW_WINDOW:
			browser->GetMainFrame()->LoadURL(target_url);
			return true; //cancel create    
		}

		return false;
	}

	virtual bool DoClose(CefRefPtr<CefBrowser> browser) 
	{ 
		::ShowWindow(browser->GetHost()->GetWindowHandle(), SW_HIDE);
		::SetParent(browser->GetHost()->GetWindowHandle(), GetDesktopWindow());

		if (browser && browser.get())
		{
			browser->GetHost()->CloseBrowser(false);
		}

		return false; 
	}

#endif

#if 1
	//CefDisplayHandler
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler()  
	{
		return this;
	}

	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) 
	{

		// Set the frame window title bar
		CefWindowHandle hwnd = browser->GetHost()->GetWindowHandle();
		if (m_BrowserId == browser->GetIdentifier())
		{
			// The frame window will be the parent of the browser window
			hwnd = GetParent(hwnd);
		}
		SetWindowText(hwnd, std::wstring(title).c_str());

	}
#endif

#if 1
	//CefLoadHandler
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() 
	{ 
		return this; 
	}

	// Returns a data: URI with the specified contents.
	std::string GetDataURI(const std::string& data, const std::string& mime_type) 
	{
		return "data:" + mime_type + ";base64," + CefURIEncode(CefBase64Encode(data.data(), data.size()), false).ToString();
	}

	virtual void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
	{
		// Don't display an error for downloaded files.
		if (errorCode == ERR_ABORTED)
			return;

		// Display a load error message using a data: URI.
		std::stringstream ss;
		ss << "<html><body bgcolor=\"white\">"
			"<h2>Failed to load URL "
			<< std::string(failedUrl) << " with error " << std::string(errorText)
			<< " (" << errorCode << ").</h2></body></html>";

		frame->LoadURL(GetDataURI(ss.str(), "text/html"));
	}
#endif

#if 1
	//CefRequestHandler+CefResourceRequestHandler 
	//[需要同时继承实现，否则回调ResourceRequest不生效]
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler()
	{ 
		return this; 
	}

	virtual CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_navigation,
		bool is_download,
		const CefString& request_initiator,
		bool& disable_default_handling)
	{
		return this;
	}

	std::string ReadPostElementBytes(CefRefPtr<CefPostDataElement> element)
	{
		std::string sBytes;
		if (element->GetType() == PDE_TYPE_BYTES)
		{
			size_t byte_count = element->GetBytesCount();
			char* bytes = new char[byte_count + 1];
			element->GetBytes(byte_count + 1, (void*)(bytes));
			bytes[byte_count] = '\0';

			sBytes = bytes;
			delete[] bytes;
		}

		return sBytes;
	}
	virtual ReturnValue OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,CefRefPtr<CefRequest> request,CefRefPtr<CefCallback> callback) 
	{
		if (m_pParent && m_pParent->GetSafeHwnd())
		{
			CefString url = request->GetURL();
			CString strurl; strurl = url.ToWString().c_str();
			if (strurl.Find(_T("updateaction")) != -1)
			{
				CefString method = request->GetMethod();
				CString strmethod; strmethod = method.ToWString().c_str();
				if (strmethod.Find(_T("POST")) != -1)
				{
					CString strHead, strBody;

					CefRequest::HeaderMap headerMap;
					request->GetHeaderMap(headerMap);
					for (CefRequest::HeaderMap::iterator itHead = headerMap.begin(); itHead != headerMap.end(); ++itHead)
					{
						CefString cstrKey = itHead->first;
						CefString cstrValue = itHead->second;

						strHead += cstrKey.ToWString().c_str();
						strHead += _T(":");
						strHead += cstrValue.ToWString().c_str();
						strHead += _T("\n\n");
					}

					CefRefPtr<CefPostData> PostData = request->GetPostData();
					CefPostData::ElementVector elementVec;
					PostData->GetElements(elementVec);
					for (CefPostData::ElementVector::iterator itElem = elementVec.begin(); itElem != elementVec.end(); ++itElem)
					{
						CString strElemBytes;
						WCHAR* pUniElemBytes(NULL);
						std::string sElemBytes = ReadPostElementBytes((*itElem));

						UTF8ToUnicode((char*)sElemBytes.c_str(), &pUniElemBytes);
						strElemBytes = pUniElemBytes;

						strBody += strElemBytes;
						strBody += _T("\n\n");
					}

					if (strBody.Find(_T("Template")) != -1)
					{
						m_strHead = strHead; m_strBody = strBody;
						::PostMessage(m_pParent->GetSafeHwnd(), WM_POSTREQUEST_SAVE, (WPARAM)m_bLocalList, (LPARAM)0);//直接发送CString数据会导致程序退出
					}
				}

				//本地单，不需要提交
				if (m_bLocalList)
					return RV_CANCEL;//取消请求
			}
		}

		return RV_CONTINUE;//继续请求
	}

	virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,CefRefPtr<CefRequest> request,CefRefPtr<CefResponse> response,URLRequestStatus status,int64 received_content_length)
	{
	}

#endif

	IMPLEMENT_REFCOUNTING(CWebClient);
};