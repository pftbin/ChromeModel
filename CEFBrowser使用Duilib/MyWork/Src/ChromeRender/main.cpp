#include "cef_app.h"
#include "cef_browser.h"
#include "cef_client.h"
#include "wrapper/cef_closure_task.h"
#include "wrapper/cef_helpers.h"
#include <Windows.h>

class CMyRenderApp
	: public CefApp
	, public CefBrowserProcessHandler
	, public CefRenderProcessHandler
{
public:
	CMyRenderApp()
	{
		m_strExtraInfo = "Extra String From Client...";
	}
	virtual ~CMyRenderApp(void) 
	{
	}

protected:
	CefString		m_strExtraInfo;

public:
	//CefApp
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
	{
		return this;
	}
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
	{
		return this;
	}

	//CefBrowserProcessHandler
	virtual void OnContextInitialized()
	{
		// The browser process UI thread has been initialized...
	}

	//CefRenderProcessHandler
	virtual void OnWebKitInitialized()
	{
		//ʹ��JSע��ȫ�ֶ���custom_object��
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

	virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
	{
		//�ض�ֵ��extra_info�ֵ��ȡ
		m_strExtraInfo = extra_info->GetString(CefString("input_extra"));
	}

	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
	{
#if 1
		context->Enter();
		CefRefPtr<CefV8Value> window = context->GetGlobal();

		//Ϊ��ҳȫ�ֶ����ض�ֵ
		CefRefPtr<CefV8Value> ObjectDemo = window->GetValue(CefString("custom_object"));
		if (ObjectDemo->IsObject())
		{
			CefRefPtr<CefV8Value> xmlValue = CefV8Value::CreateString(m_strExtraInfo);
			ObjectDemo->SetValue(CefString("varinfo"), xmlValue, CefV8Value::PropertyAttribute::V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		}
		context->Exit();
#endif
	}

	IMPLEMENT_REFCOUNTING(CMyRenderApp);
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CefMainArgs main_args(hInstance);
//	CefRefPtr<CefApp> app;								//Ĭ��CefApp
	CefRefPtr<CMyRenderApp> app(new CMyRenderApp());	//�̳�CefApp֧���޸�window����
	
	return CefExecuteProcess(main_args, app.get(), nullptr);
}