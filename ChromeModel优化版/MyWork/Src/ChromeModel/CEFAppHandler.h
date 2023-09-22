#pragma once
#include <cef_app.h>

class CEFAppHandler 
	: public CefApp
	, public CefBrowserProcessHandler
	, public CefRenderProcessHandler
{
public:
	CEFAppHandler();
	virtual ~CEFAppHandler(void);

public:
	//CefApp
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

	//CefBrowserProcessHandler
	virtual void OnContextInitialized() override;

	//CefRenderProcessHandler
	virtual void OnWebKitInitialized() override;
	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info) override;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

private:
	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CEFAppHandler);

	// Include the default locking implementation.
	//IMPLEMENT_LOCKING(CEFAppHandler); //线程锁定义,当前版本无,不使用

private:
	CefString		m_strExtraInfo;
};

