#pragma once

#include "stdafx.h"
#include "CEFV8HandlerEx.h"

class MyV8Accessor
	: public CefV8Accessor
{
public:
	MyV8Accessor() {}

	virtual bool Get(const CefString& name,
		const CefRefPtr<CefV8Value> object,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception) override {
		if (name == "myval") {
			// Return the value.
			retval = CefV8Value::CreateString(myval_);
			return true;
		}

		// Value does not exist.
		return false;
	}

	virtual bool Set(const CefString& name, const CefRefPtr<CefV8Value> object, const CefRefPtr<CefV8Value> value, CefString& exception) override {
		if (name == "myval") {
			if (value->IsString()) {
				// Store the value.
				myval_ = value->GetStringValue();
			}
			else {
				// Throw an exception.
				exception = "Invalid value type";
			}
			return true;
		}

		// Value does not exist.
		return false;
	}

	// Variable used for storing the value.
	CefString myval_;

	// Provide the reference counting implementation for this class.
	IMPLEMENT_REFCOUNTING(MyV8Accessor);
};

class CCEFAppHandler 
	: public CefApp
	, public CefBrowserProcessHandler
	, public CefRenderProcessHandler
{
public:
	CCEFAppHandler();
	~CCEFAppHandler();

	// CefApp methods:
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
	{
		return this;
	}
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override
	{
		return this;
	}
	virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override
	{
		//加载flash插件
		command_line->AppendSwitchWithValue("--ppapi-flash-path", "ppflash/18_0_0_209/pepflashplayer32_18_0_0_209.dll");
		//manifest.json中的version
		command_line->AppendSwitchWithValue("--ppapi-flash-version", "18.0.0.209");
		command_line->AppendSwitch("--disable-extensions");
	}
	
	
	// CefBrowserProcessHandler methods:
	virtual void OnContextInitialized() override;

	//CefRenderProcessHandler methods
	virtual void OnWebKitInitialized() override;
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)  override;
	virtual void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;


private:
	CefRefPtr<CCEFV8HandlerEx> v8Handler_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CCEFAppHandler);
};