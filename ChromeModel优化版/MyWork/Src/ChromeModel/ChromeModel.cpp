// ChromeModel.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "ChromeModel.h"

#include "cef_app.h"
#include "cef_client.h"

#include "CEFViewDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CChromeModelApp

BEGIN_MESSAGE_MAP(CChromeModelApp, CWinApp)
END_MESSAGE_MAP()


// CChromeModelApp 构造

CChromeModelApp::CChromeModelApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CChromeModelApp 对象
ChromeModel theModel;
CChromeModelApp theApp;

CEFViewDialog* g_pChromeViewDlg = NULL;
CString g_strUrl = _T("");


// CChromeModelApp 初始化

BOOL CChromeModelApp::InitInstance()
{
	return CWinApp::InitInstance();
}

int CChromeModelApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

extern "C" __declspec(dllexport) BOOL InitChromeModel()//需在主线程调用此函数
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return theModel.InitCEFModel();
}

extern "C" __declspec(dllexport) BOOL UnInitChromeModel()//需在主线程调用此函数
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return theModel.UnInitCEFModel();
}

extern "C" __declspec(dllexport) BOOL SetWindowParam(const CString & strUrl)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_strUrl = strUrl;

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL CreateChromeWindow(CWnd * pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	if (g_pChromeViewDlg == NULL)
		g_pChromeViewDlg = new CEFViewDialog();
	g_pChromeViewDlg->SetHttpURL(g_strUrl);
	g_pChromeViewDlg->Create(CEFViewDialog::IDD, pParentWnd);
	g_pChromeViewDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL DoModalChromeWindow(CWnd * pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CEFViewDialog ChromeViewDlg;
	ChromeViewDlg.SetHttpURL(g_strUrl);
	ChromeViewDlg.SetParent(pParentWnd);
	if (ChromeViewDlg.DoModal() != IDABORT)
	{
		ChromeViewDlg.DetachDelegate();
	}

	return TRUE;
}

extern "C" __declspec(dllexport) CWnd * GetChromeWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return g_pChromeViewDlg;
}

extern "C" __declspec(dllexport) void DestroyChromeWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (g_pChromeViewDlg && ::IsWindow(g_pChromeViewDlg->m_hWnd))
	{
		g_pChromeViewDlg->DestroyWindow();
		g_pChromeViewDlg = NULL;
	}
}




