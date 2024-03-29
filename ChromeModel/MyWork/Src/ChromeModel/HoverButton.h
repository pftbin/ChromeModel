#if !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
#define AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HoverButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHoverButton by Niek Albers
// Thanks to some people for the tooltip.
// A cool CBitmapButton derived class with 3 states,
// Up/Down/Hover.
class CHoverButton : public CBitmapButton
{
	DECLARE_DYNAMIC(CHoverButton);

	// Construction
public:
	CHoverButton();
	void SetToolTipText(CString* spText, BOOL bActivate = TRUE);
	void SetToolTipText(int nId, BOOL bActivate = TRUE);
	void SetPressed(BOOL bPress);
	void SetCheckButton(BOOL bSet);
	BOOL IsPressed(){return m_bPress;}

// Attributes
protected:
	virtual void PreSubclassWindow();
	void ActivateTooltip(BOOL bActivate = TRUE);
	BOOL m_bHover;						// indicates if mouse is over the button
	CSize m_ButtonSize;					// width and height of the button
//	CBitmap mybitmap;
	CImageList m_ImageList;
	BOOL m_bTracking;
	BOOL m_bPress;
	BOOL m_bCheckButton;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoverButton)
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetBitmap(HBITMAP hBmp, COLORREF cMask, BOOL bMask);
	HBITMAP GetBitmap() { return m_hBmp; }
	BOOL LoadBitmap(UINT bitmapid,int cx,COLORREF cMask,BOOL bMask);
	void RedrawButton();
	virtual ~CHoverButton();

	// Generated message map functions
protected:
	CToolTipCtrl m_ToolTip;
	CDC			m_dcBk;
	CBitmap		m_bmpBk;
	CBitmap     *m_bmpBkOld;
	BOOL        m_bRedraw;
	HBITMAP		m_hBmp;
	void InitToolTip();
	//{{AFX_MSG(CHoverButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnMouseHover(WPARAM wparam, LPARAM lparam) ;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOVERBUTTON_H__16C6D980_BD45_11D3_BDA3_00104B133581__INCLUDED_)
