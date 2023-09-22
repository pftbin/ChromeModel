#include "pch.h"
#include "CEFAppHandler.h"

CEFAppHandler::CEFAppHandler()
{
	m_strExtraInfo = "Extra String From Client...";
}

CEFAppHandler::~CEFAppHandler(void) 
{
};

//CefBrowserProcessHandler
void CEFAppHandler::OnContextInitialized()
{
	// The browser process UI thread has been initialized...
}

//CefRenderProcessHandler
void CEFAppHandler::OnWebKitInitialized()
{
	//使用JS注册全局对象（custom_object）
#if 1
	std::string extensionCode =
		"var custom_object;"
		"if (!custom_object)"
		"  custom_object = {};"
		"(function() {"
		"custom_object.varinfo='null';"
		"})();";

	// Register the extension.
	CefRegisterExtension("v8/test", extensionCode, nullptr);
#endif
}

void CEFAppHandler::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
{
	//特定值从extra_info字典获取
	m_strExtraInfo = extra_info->GetString(CefString("input_extra"));
}

void CEFAppHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
#if 1
	context->Enter();
	CefRefPtr<CefV8Value> window = context->GetGlobal();

	//为网页全局对象赋特定值
	CefRefPtr<CefV8Value> ObjectDemo = window->GetValue(CefString("custom_object"));
	if (ObjectDemo->IsObject())
	{
		CefRefPtr<CefV8Value> xmlValue = CefV8Value::CreateString(m_strExtraInfo);
		ObjectDemo->SetValue(_T("varinfo"), xmlValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
	}
	context->Exit();
#endif
}

