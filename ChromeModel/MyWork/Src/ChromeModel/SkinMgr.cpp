#include "pch.h"
#include ".\skinmgr.h"

CSkinMgr::CSkinMgr(void)
{
	TCHAR szPath[MAX_PATH]={0};
	::GetModuleFileName(AfxGetInstanceHandle(), szPath, MAX_PATH);
	int nLen = lstrlen(szPath);
	for(int i=nLen-1; i>=0; i--)
	{
		if(szPath[i] == _T('\\'))
		{
			szPath[i] = 0;
			break;
		}
	}
	m_strAppFolder = szPath;
	m_strIniFile = m_strAppFolder + _T("\\skin.ini");
}

CSkinMgr::~CSkinMgr(void)
{
	UnloadBitmap();
	UnloadIcon();
}

CString CSkinMgr::GetBitmapPath(LPCTSTR szID)
{
	TCHAR szBmpPath[MAX_PATH]={0};
	CString strID = szID;
	strID.MakeLower();	
	GetPrivateProfileString(_T("SkinPath"), strID, _T(""), szBmpPath, MAX_PATH, m_strIniFile);
	CString strPath = m_strAppFolder + _T("\\");
	strPath += szBmpPath;
	return strPath;
}

HBITMAP CSkinMgr::LoadBitmap(LPCTSTR szID,  SIZE& size)
{
	CString strID = szID;
	strID.MakeLower();
	size.cx = size.cy = 0;
	BitmapMap::iterator iter = m_bmpMap.find(strID);
	if(iter != m_bmpMap.end())
	{
		size = iter->second.size;
		return iter->second.hBitmap;
	}

	HBITMAP hBmp = NULL;
	TCHAR szBmpPath[MAX_PATH]={0};
	DWORD dwRet = GetPrivateProfileString(_T("SkinPath"), strID, _T(""), szBmpPath, MAX_PATH, m_strIniFile);
	if(dwRet > 0)
	{
		CString strBmpPath = m_strAppFolder;
		strBmpPath += _T("\\"); strBmpPath += szBmpPath;
		hBmp = (HBITMAP)::LoadImage(NULL, strBmpPath, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	}

/*	if(hBmp == NULL)
	{
		HINSTANCE hInst = AfxGetResourceHandle();
		hBmp = (HBITMAP)::LoadImage(hInst, strID, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
	}*/

	if(hBmp != NULL)
	{
		BITMAP bitmap;
		if(CBitmap::FromHandle(hBmp)->GetBitmap(&bitmap))
		{
			size.cx = bitmap.bmWidth;
			size.cy = bitmap.bmHeight;
		}
		else
		{
			DeleteObject(hBmp);
			return NULL;
		}

		BitmapInfo bi;
		bi.hBitmap = hBmp;
		bi.size = size;
		m_bmpMap.insert(std::make_pair(strID, bi));
	}

	return hBmp;
}

HICON CSkinMgr::LoadIcon(LPCTSTR szID, SIZE size)
{
	CString strID = szID;
	strID.MakeLower();
	IconMap::iterator iter = m_iconMap.find(strID);
	if(iter != m_iconMap.end())
	{
		size = iter->second.size;
		return iter->second.hIcon;
	}

	HICON hIcon = NULL;
	TCHAR szIconPath[MAX_PATH];
	DWORD dwRet = GetPrivateProfileString(_T("SkinPath"), strID, _T(""), szIconPath, MAX_PATH, m_strIniFile);
	if(dwRet > 0)
	{
		CString strIconPath = m_strAppFolder;
		strIconPath += _T("\\"); strIconPath += szIconPath;
		hIcon = (HICON)::LoadImage(NULL, strIconPath, IMAGE_ICON, size.cx, size.cy, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	}

	if(hIcon == NULL)
	{
		HINSTANCE hInst = AfxGetResourceHandle();
		hIcon = (HICON)::LoadImage(hInst, strID, IMAGE_ICON, size.cx, size.cy, LR_DEFAULTSIZE);
	}

	if(hIcon != NULL)
	{
		IconInfo ii;
		ii.hIcon = hIcon;
		ii.size = size;
		m_iconMap.insert(std::make_pair(strID, ii));
	}

	return hIcon;
}

void CSkinMgr::UnloadBitmap(LPCTSTR szID)
{
	CString strID = szID;
	strID.MakeLower();
	BitmapMap::iterator iter = m_bmpMap.find(strID);
	if(iter != m_bmpMap.end())
	{
		DeleteObject(iter->second.hBitmap);
		m_bmpMap.erase(iter);
	}
}

void CSkinMgr::UnloadIcon(LPCTSTR szID)
{
	CString strID = szID;
	strID.MakeLower();
	IconMap::iterator iter = m_iconMap.find(strID);
	if(iter != m_iconMap.end())
	{
		DeleteObject(iter->second.hIcon);
		m_iconMap.erase(iter);
	}
}

void CSkinMgr::UnloadBitmap(void)
{
	BitmapMap::iterator iter = m_bmpMap.begin();
	for(; iter != m_bmpMap.end(); ++iter)
	{
		DeleteObject(iter->second.hBitmap);
	}
	m_bmpMap.clear();
}

void CSkinMgr::UnloadIcon(void)
{
	IconMap::iterator iter = m_iconMap.begin();
	for(; iter != m_iconMap.end(); ++iter)
	{
		DeleteObject(iter->second.hIcon);
	}
	m_iconMap.clear();
}

CSkinMgr* CSkinMgr::Instance(void)
{
	static CSkinMgr skinMgr;
	return &skinMgr;
}
