#pragma once
#include "TBar.h"

//#include "AutoFitDlg.h"
// CSkinDlg �Ի���

class CSkinDlg : public CDialog
{
	DECLARE_DYNAMIC(CSkinDlg)

public:
	CSkinDlg(int nIDD, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSkinDlg();

	CTBar& GetTitleBar(void) { return m_tBar; }
	virtual void SetBkgndColor(COLORREF color);
	virtual void SetBkgndBmp(HBITMAP hBmp);

protected:
	CTBar				m_tBar;
	COLORREF			m_crBack;
	BITMAP				m_bmBkgnd;
	HBITMAP				m_hBmpBkgnd;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
