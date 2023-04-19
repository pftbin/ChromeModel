// ChromeModel.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "ChromeModel.h"

#include "cef_app.h"
#include "cef_client.h"

#include "ChromeUIDlg.h"

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

ChromeUIDlg* g_pChromeWnd = NULL;
CString g_strUrl = _T("");
CString g_strTemplateXml = _T("");
CString g_strMosId = _T("");
CString g_strRoomName = _T("");
BOOL    g_bLocalList = FALSE;


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

extern "C" __declspec(dllexport) BOOL SetWindowParam(const CString & strUrl, const CString & strTemplateXml, const CString & strMosId, const CString & strRoomName, const BOOL & bLocalList)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_strUrl = strUrl;
	g_strTemplateXml = strTemplateXml;
	g_strMosId = strMosId;
	g_strRoomName = strRoomName;
	g_bLocalList = bLocalList;

	return TRUE;
}

extern "C" __declspec(dllexport) BOOL CreateChromeWindow(CWnd * pParentWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pChromeWnd == NULL)
		g_pChromeWnd = new ChromeUIDlg();
	
	g_pChromeWnd->m_strUrl = g_strUrl;
	g_pChromeWnd->m_strTemplateXml = g_strTemplateXml;
	g_pChromeWnd->m_strMosId = g_strMosId;
	g_pChromeWnd->m_strRoomName = g_strRoomName;
	g_pChromeWnd->m_bLocalList = g_bLocalList;
	g_pChromeWnd->Create(ChromeUIDlg::IDD, pParentWnd);
	g_pChromeWnd->ShowWindow(SW_HIDE);

	return TRUE;
}

extern "C" __declspec(dllexport) CWnd * GetChromeWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return g_pChromeWnd;
}

extern "C" __declspec(dllexport) void DestroyChromeWindow()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (g_pChromeWnd && ::IsWindow(g_pChromeWnd->m_hWnd))
	{
		g_pChromeWnd->DestroyWindow();
		g_pChromeWnd = NULL;
	}
}

extern "C" __declspec(dllexport) BOOL GetWindowReqSaveData(CString & strHeadContent, CString & strBodyContent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pChromeWnd == NULL) return FALSE;

	return g_pChromeWnd->GetReqSaveData(strHeadContent, strBodyContent);
}

extern "C" __declspec(dllexport) BOOL ExChangeDataNotify(int nType, BOOL& bDataNotify)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pChromeWnd == NULL) return FALSE;

	if (nType == 0) //Get
		bDataNotify = g_pChromeWnd->m_bDataNotify;
	if (nType == 1) //Set
		g_pChromeWnd->m_bDataNotify = bDataNotify;

	return TRUE;
}




