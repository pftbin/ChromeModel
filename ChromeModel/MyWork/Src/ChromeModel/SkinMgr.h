#pragma once
#include <map>

class CSkinMgr
{
public:
	CString GetBitmapPath(LPCTSTR strID);
	HBITMAP LoadBitmap(LPCTSTR strID,  SIZE& size);
	HICON LoadIcon(LPCTSTR strID, SIZE size);
	void UnloadBitmap(LPCTSTR strID);
	void UnloadIcon(LPCTSTR strID);
	void UnloadBitmap(void);
	void UnloadIcon(void);
	static CSkinMgr* Instance(void);

protected:
	CSkinMgr(void);
	~CSkinMgr(void);

	struct BitmapInfo {
		HBITMAP	hBitmap;
		SIZE	size;
	};
	struct IconInfo {
		HICON	hIcon;
		SIZE	size;
	};

private:
	typedef std::map<CString, BitmapInfo>	BitmapMap;
	typedef std::map<CString, IconInfo>		IconMap;

	BitmapMap	m_bmpMap;
	IconMap		m_iconMap;
	CString		m_strAppFolder;
	CString		m_strIniFile;
};
