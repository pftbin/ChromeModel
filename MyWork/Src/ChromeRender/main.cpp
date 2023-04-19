#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include <Windows.h>

class CMyRenderApp
	: public CefApp
	, public CefBrowserProcessHandler
	, public CefRenderProcessHandler
{
public:
	CMyRenderApp()
	{
		m_infoTemplateXml = "TemplateXml init in Processapp";
		m_infoMosId = "MosId init in Processapp";
		m_infoRoomName = "RoomName init in Processapp";
	}
	virtual ~CMyRenderApp(void) 
	{
	}

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

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
	{
		m_infoTemplateXml = extra_info->GetString(CefString("info_TemplateXml"));
		m_infoMosId = extra_info->GetString(CefString("info_MosId"));
		m_infoRoomName = extra_info->GetString(CefString("info_RoomName"));
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
	{
#if 1
		context->Enter();
		CefRefPtr<CefV8Value> window = context->GetGlobal();

		//fck_ext_hubeidemo.xml赋值
		CefRefPtr<CefV8Value> ObjectDemo = window->GetValue(CefString("fck_ext_hubeidemo"));
		if (ObjectDemo->IsObject())
		{
			CefRefPtr<CefV8Value> xmlValue = CefV8Value::CreateString(m_infoTemplateXml);
			ObjectDemo->SetValue(CefString("xml"), xmlValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		}

		//playrooms.mosid + playrooms.playroomname 赋值
		CefRefPtr<CefV8Value> ObjectPlayRooms = window->GetValue(CefString("playrooms"));
		if (ObjectPlayRooms->IsObject())
		{
			CefRefPtr<CefV8Value> mosidValue = CefV8Value::CreateString(m_infoMosId);
			ObjectPlayRooms->SetValue(CefString("mosid"), mosidValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);

			CefRefPtr<CefV8Value> nameValue = CefV8Value::CreateString(m_infoRoomName);
			ObjectPlayRooms->SetValue(CefString("playroomname"), nameValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		}

		context->Exit();
#endif
	}

	IMPLEMENT_REFCOUNTING(CMyRenderApp);
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CefMainArgs main_args(hInstance);
//	CefRefPtr<CefApp> app;								//默认CefApp
	CefRefPtr<CMyRenderApp> app(new CMyRenderApp());	//继承CefApp支持修改window对象
	
	return CefExecuteProcess(main_args, app.get(), nullptr);
}