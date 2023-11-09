#pragma once
// CAirMsgBox �Ի���
#include "SkinDlg.h"
#include "SkinMgr.h"
#include "HoverButton.h"
#include "Resource.h"

class CAirMsgBox : public CSkinDlg
{
	DECLARE_DYNAMIC(CAirMsgBox)

public:
	CAirMsgBox(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAirMsgBox();

// �Ի�������
	enum { IDD = IDD_DIALOG_MSG};
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	void SetInitParam(CString strTitle,CString strMsg,UINT uID){m_strTitle = strTitle;m_strMsg = strMsg;m_uID = uID;}
	BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
private:
	CString m_strTitle;
	CString m_strMsg;
	UINT    m_uID;
	int     m_nCharsRow;      //ÿ������
	int     m_nCountRow;      //������
	CFont	*m_pftYaheipx;
	
	void AdjustDlgPosition();
	void SetButton();
	void SetPicture();
	void FormatMsgContont();
public:
	CHoverButton m_btnA;
	CHoverButton m_btnB;
	CStatic      m_stcMsg;
	CStatic*     m_pStaPic;

	CBitmap					m_BitmapCloseUp;
	CBitmap					m_BitmapCloseHover;
	CBitmap					m_BitmapCloseDown;
	CBitmap					m_BitmapTitleBarL;
	CBitmap					m_BitmapTitleBarC;
	CBitmap					m_BitmapTitleBarR;
	CBitmap					m_BitmapDialogBk;

	CBitmap					m_BitmapOK;
	CBitmap					m_BitmapCancel;
	CBitmap					m_BitmapStop;
	CBitmap					m_BitmapQuestion;
	CBitmap					m_BitmapWarnning;
};
