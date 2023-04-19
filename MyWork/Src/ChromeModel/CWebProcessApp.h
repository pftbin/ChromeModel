#pragma once
#include <cef_app.h>

class CWebProcessApp 
	: public CefApp
	, public CefBrowserProcessHandler
	, public CefRenderProcessHandler
{
public:
	CWebProcessApp();
	virtual ~CWebProcessApp(void);

protected:
	CefString		m_infoTemplateXml;
	CefString		m_infoMosId;
	CefString		m_infoRoomName;

public:
	//CefBrowserProcessHandler
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() 
	{
		return this;
	}

	virtual void OnContextInitialized() 
	{
		// The browser process UI thread has been initialized...
	}

	//CefRenderProcessHandler
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() 
	{
		return this;
	}

	virtual void OnWebKitInitialized() 
	{
		//此函数注册的对象属于全局对象（变量）
#if 1
		std::string extensionCode =
			"var fck_ext_hubeidemo;"
			"if (!fck_ext_hubeidemo)"
			"  fck_ext_hubeidemo = {};"
			"(function() {"
			"fck_ext_hubeidemo.xml='templatexml init in OnWebKitInitialized';"
			"})();"
			"var playrooms;"
			"if (!playrooms)"
			"  playrooms = {};"
			"(function() {"
			"playrooms.mosid='mosid init in OnWebKitInitialized';"
			"playrooms.playroomname='playroomname init in OnWebKitInitialized';"
			"})();";

		// Register the extension.
		CefRegisterExtension("v8/test", extensionCode, nullptr);
#endif
	}

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,CefRefPtr<CefDictionaryValue> extra_info)
	{
		m_infoTemplateXml = extra_info->GetString(CefString("info_TemplateXml"));
		m_infoMosId = extra_info->GetString(CefString("info_MosId"));
		m_infoRoomName = extra_info->GetString(CefString("info_RoomName"));
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,CefRefPtr<CefFrame> frame,CefRefPtr<CefV8Context> context) 
	{
#if 1
		context->Enter();
		CefRefPtr<CefV8Value> window = context->GetGlobal();

		//fck_ext_hubeidemo.xml赋值
		CefRefPtr<CefV8Value> ObjectDemo = window->GetValue(CefString("fck_ext_hubeidemo"));
		if (ObjectDemo->IsObject())
		{
			CefRefPtr<CefV8Value> xmlValue = CefV8Value::CreateString(m_infoTemplateXml);
			ObjectDemo->SetValue(_T("xml"), xmlValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		}

		//playrooms.mosid + playrooms.playroomname 赋值
		CefRefPtr<CefV8Value> ObjectPlayRooms = window->GetValue(CefString("playrooms"));
		if (ObjectPlayRooms->IsObject())
		{
			CefRefPtr<CefV8Value> mosidValue = CefV8Value::CreateString(m_infoMosId);
			ObjectPlayRooms->SetValue(_T("mosid"), mosidValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);

			CefRefPtr<CefV8Value> nameValue = CefV8Value::CreateString(m_infoRoomName);
			ObjectPlayRooms->SetValue(_T("playroomname"), nameValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		}

		context->Exit();
#endif
	}

	IMPLEMENT_REFCOUNTING(CWebProcessApp);
};

