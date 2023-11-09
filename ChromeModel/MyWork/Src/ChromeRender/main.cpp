#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_process_message.h"

#include "include/cef_client.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include <Windows.h>

// MyV8Accessor
#include "include/cef_v8.h"
class MyV8Accessor : public CefV8Accessor
{
public:
	MyV8Accessor() {}
	CefString myobject_myvalue = "1";

	virtual bool Get(const CefString& name, const CefRefPtr<CefV8Value> object, CefRefPtr<CefV8Value>& retval, CefString& exception) override
	{
		if (name == "myvalue")
		{
			retval = CefV8Value::CreateString(myobject_myvalue.ToString());
			return true;
		}

		return false;
	}

	virtual bool Set(const CefString& name, const CefRefPtr<CefV8Value> object, const CefRefPtr<CefV8Value> value, CefString& exception) override
	{
		if (name == "myvalue")
		{
			if (value->IsString())
			{
				myobject_myvalue = value->GetStringValue().ToString();
			}
			else
				exception = "TypeError~";
			return true;
		}

		return false;
	}
private:
	IMPLEMENT_REFCOUNTING(MyV8Accessor);
};

//CMyRenderApp
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
		m_bIsEditing = FALSE;
	}
	virtual ~CMyRenderApp(void) 
	{
	}

protected:
	CefString		m_infoTemplateXml;
	CefString		m_infoMosId;
	CefString		m_infoRoomName;
	BOOL			m_bIsEditing;

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

		//测试注册对象[注册js函数同理]
		//m_V8Accessor = new MyV8Accessor();
		//CefRefPtr<CefV8Value> myobj = CefV8Value::CreateObject(m_V8Accessor, nullptr);
		//myobj->SetValue("myvalue", V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_NONE);
		//window->SetValue("myobject", myobj, V8_PROPERTY_ATTRIBUTE_NONE);

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

	virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
	{
		const std::string& messageName = message->GetName();
		if (messageName == "ask_checksavestate")
		{
			//测试收到消息
			//CefRefPtr<CefFrame> frame = browser->GetMainFrame();
			//frame->ExecuteJavaScript("alert('Recv message from browser process')", frame->GetURL(), 0);

			//检查网页上指定对象的值
			CefRefPtr<CefFrame> frame = browser->GetMainFrame();
			CefRefPtr<CefV8Context> context = frame->GetV8Context();
			context->Enter();
			CefRefPtr<CefV8Value> window = context->GetGlobal()->GetValue("isEditing");
			if (window && window->IsObject())
			{
				CefRefPtr<CefV8Value> v8MyValue = window->GetValue("value");
				if (v8MyValue->IsString())
				{
					CefString cefMyValue = v8MyValue->GetStringValue();
					m_bIsEditing = (cefMyValue.compare("false") == 0) ? FALSE : TRUE;
				}
			}
			context->Exit();
			// 发送到Client进程
			CefRefPtr<CefProcessMessage> replyMessage = CefProcessMessage::Create("reply_checksavestate");
			CefRefPtr<CefListValue> replyArgs = replyMessage->GetArgumentList();	//1-添加参数
			replyArgs->SetSize(1);
			replyArgs->SetString(0, m_bIsEditing?("1"):("0"));
			browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, replyMessage);	//2-发送

			return true;
		}
		return false;
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