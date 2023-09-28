#include "stdafx.h"
#include "CEFAppHandler.h"

CCEFAppHandler::CCEFAppHandler()
{
	v8Handler_ = new CCEFV8HandlerEx;

}

CCEFAppHandler::~CCEFAppHandler()
{

}

//
void CCEFAppHandler::OnContextInitialized()
{

}

//
void CCEFAppHandler::OnWebKitInitialized()
{
	std::string app_code =
		//-----------------------------------
		//����JavaScript��Ҫ���õ�Cpp����
		"var app;"
		"if (!app)"
		"  app = {};"
		"(function() {"

		//  jsInvokeCPlusPlus 
		"  app.jsInvokeCPlusPlus = function(v1, v2) {"
		"    native function jsInvokeCPlusPlus();"
		"    return jsInvokeCPlusPlus(v1, v2);"
		"  };"

		"})();";


	// Register app extension module

	// JavaScript�����app.jsInvokeCPlusPlusʱ���ͻ�ȥͨ��CefRegisterExtensionע���CefV8Handler�б������
	// �ҵ�"v8/app"��Ӧ��CCEFV8HandlerEx���͵�������Execute����
	// ����v8Handler_��CCefClientApp��һ����Ա����
	//v8Handler_ = new CCEFV8HandlerEx();

	CefRegisterExtension("v8/app", app_code, v8Handler_);

}
void CCEFAppHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	CefRefPtr<CefV8Value> object = context->GetGlobal();// ��ȡ��window  
	CefRefPtr<CefV8Value> str = CefV8Value::CreateString("C++ created Value!");
	object->SetValue("jsValue", str, V8_PROPERTY_ATTRIBUTE_NONE);

	CefRefPtr<CefV8Accessor> accessor = new MyV8Accessor;
	CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor, nullptr);

	obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT, V8_PROPERTY_ATTRIBUTE_NONE);

	object->SetValue("myobject", obj, V8_PROPERTY_ATTRIBUTE_NONE);

}
void CCEFAppHandler::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
	v8Handler_ = nullptr;
}
