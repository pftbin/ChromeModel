#pragma once
#include "stdafx.h"
#include <list>

#include "include/cef_base.h"
#include "include/cef_browser.h"


class CCEFClientHandler 
	: public CefClient
	, public CefDisplayHandler
	, public CefLifeSpanHandler
	, public CefLoadHandler
	, public CefRequestHandler
	, public CefContextMenuHandler
{
public:
	CCEFClientHandler();
	~CCEFClientHandler();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override
	{
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
	{
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override
	{
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() override
	{
		return this;
	}
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override
	{
		return this;
	}

	// ----------------CefDisplayHandler methods:-------------------
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,const CefString& title) override;

	//---------------- CefLifeSpanHandler methods:----------------------------
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name,
		CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, CefRefPtr<CefDictionaryValue>& extra_info, bool* no_javascript_access) override;
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

	// ----------------CefLoadHandler methods:---------------------------
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,int httpStatusCode) override;
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,ErrorCode errorCode,const CefString& errorText,const CefString& failedUrl) override;

	// ----------------CefContextMenuHandler methods:---------------------------  
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;
	virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags) override;

	//
	void CloseHostBrowser(CefRefPtr<CefBrowser>browser, bool force_close) ;
	void CloseAllBrowsers(bool force_close);
	bool IsClosing() const;
	void ShowDevelopTools(CefRefPtr<CefBrowser> browser,const CefPoint& inspect_element_at);
	void CloseDevelopTools(CefRefPtr<CefBrowser> browser);

public:
	HWND hWnd_; //接收消息的句柄
	CefString strTitle_; //网址标题

	// List of existing browser windows. Only accessed on the CEF UI thread.
	typedef std::vector<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList browser_list_;

private:
	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCEFClientHandler);
	//新版本不再使用以下宏定义
	//IMPLEMENT_LOCKING(CCefClientHandler);
};


