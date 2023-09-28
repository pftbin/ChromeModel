#pragma once
#include "resource.h"
#include "CEFWebkit.h"

class CEFWebkitBrowserWnd : public WindowImplBase
{
public:
    CEFWebkitBrowserWnd();
    ~CEFWebkitBrowserWnd();

    virtual LPCTSTR GetWindowClassName() const      { return _T("CEFWebkitBrowserWnd"); }
    virtual CDuiString GetSkinFile()                { return _T("skin.xml"); }
    virtual CDuiString GetSkinFolder()              { return _T("Skin"); }

	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

	virtual void OnClick(TNotifyUI& msg) override;
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    
public:
	void OnInitComplate();
	void SwitchUIState();

	//
	void OnTitleChanged(int nWebBrowserID,const CefString str);
	void OnAfterCreate(int nWebBrowserID);
	void OnBrowserClose(int nBrowserID);
	void OnWebLoadEnd(WPARAM wparam,LPARAM lparam);
	void OnWebLoadStart(WPARAM wParam, LPARAM lParam);
	COptionUI* GetActiveOption() const;
	
public:
    wstring					strURL_;
    wstring					strTitle_;
    CCEFWebkitUI*			pWKEWebkitCtrl_; 

private:
	CEditUI*				pURLEditCtrl_;
	CLabelUI*				pWebStateCtrl_;
	CHorizontalLayoutUI*	pWebTabContainer_;
	CButtonUI*				pGoBackCtrl_;			
	CButtonUI*				pGoForwardCtrl_;		

	class COptionTag
	{
	public:
		int nID_;
		COptionTag::COptionTag(int nID) :nID_(nID)
		{
		};
	};
};
