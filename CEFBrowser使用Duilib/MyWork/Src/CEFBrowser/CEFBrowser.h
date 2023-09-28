
// CEFBrowser.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once
#include "CEFAppHandler.h"
#include "MiniDumper.h"
#include "CEFWebkitBrowser.h"

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'stdafx.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CCEFBrowserApp:
// 有关此类的实现，请参阅 CEFBrowser.cpp
//

class CCEFBrowserApp : public CWinApp
{
public:
	CCEFBrowserApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int  ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CCEFBrowserApp theApp;
