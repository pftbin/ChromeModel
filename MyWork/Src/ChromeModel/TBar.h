// ==========================================================================
// TBar.h : Task Bar Replacement Class
// ==========================================================================
#ifndef ___TBAR_H___
#define ___TBAR_H___
#pragma once


class CTBar : public CWnd
{
public:
	DECLARE_DYNCREATE(CTBar)

	// Construction / Destruction
	CTBar();
	virtual ~CTBar();


protected:
	enum { ID_TIMEOUT = 900, };

	// Properties
	BOOL		m_bAutohide;
	int			m_iHeight;
	int			m_iTimeout;
	BOOL		m_bShowClose;
	BOOL		m_bShowMax;
	BOOL		m_bShowMin;
	BOOL		m_bGhostDrag;
	COLORREF	m_clrTransparent;
	HBITMAP		m_bmpCapBtns;
	HBITMAP		m_bmpCloseBtnUp;
	HBITMAP		m_bmpCloseBtnHover;
	HBITMAP		m_bmpCloseBtnDown;
	HBITMAP		m_bmpMaxBtnUp;
	HBITMAP		m_bmpMaxBtnHover;
	HBITMAP		m_bmpMaxBtnDown;
	HBITMAP		m_bmpResBtnUp;
	HBITMAP		m_bmpResBtnHover;
	HBITMAP		m_bmpResBtnDown;
	HBITMAP		m_bmpMinBtnUp;
	HBITMAP		m_bmpMinBtnHover;
	HBITMAP		m_bmpMinBtnDown;
	HBITMAP		m_bmpBkgndL;
	HBITMAP		m_bmpBkgndC;
	HBITMAP		m_bmpBkgndR;
	CSize		m_closeBtnSize;
	CSize		m_maxBtnSize;
	CSize		m_resBtnSize;
	CSize		m_minBtnSize;
	CSize		m_bkLSize;
	CSize		m_bkCSize;
	CSize		m_bkRSize;

	// Caption Button Coordinates
	CRect		m_rCloseBtn;
	CRect		m_rMaxBtn;
	CRect		m_rMinBtn;

	BOOL		m_bLButtonDown;
	BOOL		m_bCloseDown;
	BOOL		m_bMaxDown;
	BOOL		m_bMinDown;

	long		m_lLastAction;
	BOOL		m_bDragging;
	CPoint		m_ptLast;
	CPoint		m_ptOrigin;
	CPoint		m_ptLastHook;
	BOOL		m_bHidden;
	BOOL		m_bOpenAutoHide;

	HIMAGELIST	m_hImageList;
	HBITMAP		m_hDragImage;
	CDC*		m_pDC;	

	CString		m_strTitle;
	COLORREF	m_clrTitle;

	enum { MINBTN, MAXBTN, CLOSEBTN, BKGROUND };
	int			m_nLastHoverItem;

	CFont		m_font;

	// Hooks
	static HHOOK	m_hCallWndHook;
	static HHOOK	m_hMouseHook;
	static CTBar*	m_pThis;
	
public:
	// Property Accessor Functions
	int GetBarHeight(){return m_iHeight;}
	void SetBarHeight(int i);
	BOOL GetShowClose(){return m_bShowClose;}
	void SetShowClose(BOOL b);
	BOOL GetShowMin(){return m_bShowMin;}
	void SetShowMin(BOOL b);
	BOOL GetShowMax(){return m_bShowMax;}
	void SetShowMax(BOOL b);
	BOOL GetAutohide(){return m_bAutohide;}
	void SetAutohide(BOOL b){m_bAutohide = b;}
	BOOL GetGhostDrag(){return m_bGhostDrag;}
	void SetGhostDrag(BOOL b){m_bGhostDrag = b;}
	//BOOL SetCaptionButtons(UINT nID){return m_bmpCapBtns.LoadBitmap(nID);}
	/*BOOL SetCloseBtnUpBmp(UINT nID){return m_bmpCloseBtnUp.LoadBitmap(nID);}
	BOOL SetCloseBtnDownBmp(UINT nID){return m_bmpCloseBtnDown.LoadBitmap(nID);}
	BOOL SetMaxBtnUpBmp(UINT nID){return m_bmpMaxBtnUp.LoadBitmap(nID);}
	BOOL SetMaxBtnDownBmp(UINT nID){return m_bmpMaxBtnDown.LoadBitmap(nID);}
	BOOL SetResBtnUpBmp(UINT nID){return m_bmpResBtnUp.LoadBitmap(nID);}
	BOOL SetResBtnDownBmp(UINT nID){return m_bmpResBtnDown.LoadBitmap(nID);}
	BOOL SetMinBtnUpBmp(UINT nID){return m_bmpMinBtnUp.LoadBitmap(nID);}
	BOOL SetMinBtnDownBmp(UINT nID){return m_bmpMinBtnDown.LoadBitmap(nID);}*/
	BOOL SetCloseBtnBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown);
	BOOL SetMaxBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown);
	BOOL SetResBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown);
	BOOL SetMinBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown);
	BOOL SetBackGroundBmp(HBITMAP hBmpL, HBITMAP hBmpC, HBITMAP hBmpR);
	COLORREF GetTransparentColor(){return m_clrTransparent;}
	void SetTransparentColor(COLORREF clr){m_clrTransparent = clr;}
	void OpenAutoHide(BOOL bHide = TRUE);
	void SetTitle(CString strTitle, COLORREF clr)
	{ m_strTitle = strTitle, m_clrTitle = clr; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTBar)
	public:
	virtual BOOL Create(CWnd* pParentWnd, UINT nID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_VIRTUAL

protected:
	// Autohide helper functions
	void Autohide(BOOL bHide = TRUE);
	void SlideWindow(CRect& rEnd);
	
	// Dragging Helper Functions
	void StartDrag(CPoint);
	void Move(CPoint);
	void EndDrag();
	BOOL Track();
	HBITMAP CreateDragImage(CRect* pRect, BOOL bAdjustForDesktop = TRUE);
	CRect GetDesktopRect();

	void DrawGradient(CDC* pDC, CRect rect, BOOL bHorz, 
		COLORREF clrStart, COLORREF clrEnd); 

	void UpdateHoverItem(int nNewItem);

	// Generated message map functions
	//{{AFX_MSG(CTBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	virtual void OnPaint();
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

	// Callbacks
	static LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
};


//{{AFX_INSERT_LOCATION}}
#endif // ___TBAR_H___
