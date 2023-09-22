
// ChromeTester.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "ChromeTester.h"
#include "ChromeTesterDlg.h"

BOOL GetAppFolder(LPTSTR szFolder, int nBufSize)
{
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	int nLen = lstrlen(szPath);
	for (int i = nLen - 1; i >= 0; i--)
	{
		if (szPath[i] == _T('\\'))
		{
			szPath[i] = 0;
			break;
		}
	}
	if (lstrlen(szPath) >= nBufSize) return FALSE;
	lstrcpy(szFolder, szPath);
	return TRUE;
}

void Char2CString(char* pBuffer, CString& pWideString)
{
	//计算char *数组大小，以字节为单位，一个汉字占两个字节
	int charLen = strlen(pBuffer);
	//计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, NULL, 0);
	//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
	TCHAR* buf = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, pBuffer, charLen, buf, len);
	buf[len] = '\0'; //添加字符串结尾，注意不是len+1
	//将TCHAR数组转换为CString
	pWideString.Append(buf);
	//删除缓冲区
	delete[]buf;
}

BOOL	g_bCEFInitState = FALSE;
BOOL	(*g_pInitCefModel)() = NULL;
BOOL	(*g_pUnInitCefModel)() = NULL;
BOOL    (*g_pSetWindowParam)(const CString& strUrl) = NULL;
BOOL	(*g_pCreateChromeWindow)(CWnd* pParentWnd) = NULL;
BOOL	(*g_pDoModalChromeWindow)(CWnd* pParentWnd) = NULL;
CWnd*	(*g_pGetChromeWindow)() = NULL;
void	(*g_pDestroyChromeWindow)() = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChromeTesterApp

BEGIN_MESSAGE_MAP(CChromeTesterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CChromeTesterApp 构造

CChromeTesterApp::CChromeTesterApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
	m_hChromeModel = NULL;
}

CChromeTesterApp::~CChromeTesterApp()
{
}

// 唯一的 CChromeTesterApp 对象

CChromeTesterApp theApp;


// CChromeTesterApp 初始化

BOOL CChromeTesterApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

#if 1 //CEF
	TCHAR szPath[MAX_PATH];
	GetAppFolder(szPath, sizeof szPath);
	lstrcat(szPath, _T("\\ChromeModel.dll"));
	m_hChromeModel = ::LoadLibrary(szPath);
	if (m_hChromeModel == NULL)
	{
		TRACE(_T("Load ChannelControl.dll failed...\n"));
	}
	else
	{
		(FARPROC&)g_pInitCefModel = GetProcAddress(m_hChromeModel, "InitChromeModel");
		(FARPROC&)g_pUnInitCefModel = GetProcAddress(m_hChromeModel, "UnInitChromeModel");
		(FARPROC&)g_pSetWindowParam = GetProcAddress(m_hChromeModel, "SetWindowParam");
		(FARPROC&)g_pCreateChromeWindow = GetProcAddress(m_hChromeModel, "CreateChromeWindow");
		(FARPROC&)g_pDoModalChromeWindow = GetProcAddress(m_hChromeModel, "DoModalChromeWindow");
		(FARPROC&)g_pGetChromeWindow = GetProcAddress(m_hChromeModel, "GetChromeWindow");
		(FARPROC&)g_pDestroyChromeWindow = GetProcAddress(m_hChromeModel, "DestroyChromeWindow");
	}

	if (g_pInitCefModel)
	{
		g_bCEFInitState = g_pInitCefModel();
	}
#endif

	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CChromeTesterDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	// 

	return FALSE;
}

int  CChromeTesterApp::ExitInstance()
{
#if 1 //CEF
	if (g_bCEFInitState && g_pUnInitCefModel)
	{
		g_pUnInitCefModel();
		g_bCEFInitState = FALSE;
	}

	if (m_hChromeModel)
	{
		::FreeLibrary(m_hChromeModel);
		m_hChromeModel = NULL;
	}
#endif

	return CWinApp::ExitInstance();
}

