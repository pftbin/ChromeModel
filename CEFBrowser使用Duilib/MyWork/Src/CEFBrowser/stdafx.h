// stdafx.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#pragma once

// 添加要在此处预编译的标头
#include "framework.h"


//CEF
#include <cef_client.h>
#include <cef_app.h>
#include <capi/cef_app_capi.h>
#include <base/cef_bind.h>
#include <wrapper/cef_closure_task.h>
#include <cef_app.h>
#include <cef_base.h>
#include <base/cef_lock.h>
#include <wrapper/cef_helpers.h>
#pragma comment(lib, "libcef.lib")
#pragma comment(lib, "libcef_dll_wrapper.lib")

//DuiLib
#include "../DuiLib/UIlib.h"
#include "../DuiLib/Ex/ShadowWindow.h"

using namespace DuiLib;
#pragma comment(lib, "DuiLib.lib")

#define  UM_CEF_WEBLOADSTART			WM_USER+100
#define  UM_CEF_WEBLOADEND				WM_USER+101
#define  UM_CEF_WEBLOADPOPUP			WM_USER+102
#define  UM_CEF_WEBTITLECHANGE			WM_USER+104
#define  UM_CEF_AFTERCREATED			WM_USER+105
#define  UM_CEF_BROWSERCLOSE			WM_USER+106
#define  UM_CEF_POSTQUITMESSAGE			WM_USER+107
