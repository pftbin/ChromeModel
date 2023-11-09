// ==========================================================================
// TBar.cpp : Task Bar Replacement Class
// ==========================================================================
#include "pch.h"
#include "TBar.h"


// Static variable initialization
HHOOK CTBar::m_hCallWndHook = NULL;
HHOOK CTBar::m_hMouseHook = NULL;
CTBar* CTBar::m_pThis = NULL;

BEGIN_MESSAGE_MAP(CTBar, CWnd)
	//{{AFX_MSG_MAP(CTBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


IMPLEMENT_DYNCREATE(CTBar, CWnd)

//=================================================
// Construction / Destruction
//=================================================
CTBar::CTBar()
{
	m_bAutohide			= TRUE;
	m_iHeight			= 30;
	m_iTimeout			= 5000;
	m_bShowClose		= TRUE;
	m_bShowMax			= FALSE;
	m_bShowMin			= TRUE;
	m_bLButtonDown		= FALSE;
	m_bCloseDown		= FALSE;
	m_bMaxDown			= FALSE;
	m_bMinDown			= FALSE;
	m_bDragging			= FALSE;
	m_lLastAction		= 0;
	m_bHidden			= FALSE;
	m_clrTransparent	= RGB(255,0,255);
	m_hImageList		= NULL;
	m_bGhostDrag		= FALSE;
	m_bmpCapBtns		= NULL;
	m_bmpCloseBtnUp		= NULL;
	m_bmpCloseBtnHover	= NULL;
	m_bmpCloseBtnDown	= NULL;
	m_bmpMaxBtnUp		= NULL;
	m_bmpMaxBtnHover	= NULL;
	m_bmpMaxBtnDown		= NULL;
	m_bmpResBtnUp		= NULL;
	m_bmpResBtnHover	= NULL;
	m_bmpResBtnDown		= NULL;
	m_bmpMinBtnUp		= NULL;
	m_bmpMinBtnHover	= NULL;
	m_bmpMinBtnDown		= NULL;
	m_bmpBkgndL			= NULL;
	m_bmpBkgndC			= NULL;
	m_bmpBkgndR			= NULL;
	m_nLastHoverItem	= BKGROUND;
	m_bOpenAutoHide		= FALSE;

	m_closeBtnSize.SetSize(0,0);
	m_maxBtnSize.SetSize(0,0);
	m_resBtnSize.SetSize(0,0);
	m_minBtnSize.SetSize(0,0);
	m_bkLSize.SetSize(0,0);
	m_bkCSize.SetSize(0,0);
	m_bkRSize.SetSize(0,0);
	m_clrTitle = RGB(255,255,255);

	VERIFY(m_font.CreateFont(
		-15,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial")));                 // lpszFacename
}

CTBar::~CTBar()
{
	// Clean up image list
	if(m_hImageList)
	{
		ImageList_Destroy(m_hImageList);
		m_hImageList = NULL;
	}
}

//=================================================
// Overrides
//=================================================
BOOL CTBar::Create(CWnd* pParentWnd, UINT nID) 
{
	// Register a windows class
	CString strClass = AfxRegisterWndClass(CS_DBLCLKS|CS_VREDRAW|CS_HREDRAW, 
		LoadCursor(NULL, IDC_ARROW),(HBRUSH)(COLOR_BTNFACE + 1), 0);

	// Create self
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;
	return CWnd::Create(strClass, _T(""), dwStyle, 
		CRect(0,0,0,0), pParentWnd, nID, NULL);
}

//=================================================
// Windows Message Handlers
//=================================================
int CTBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Position the window
	CRect rParent;
	GetParent()->GetClientRect(&rParent);
	MoveWindow(CRect(rParent.left, rParent.top, rParent.right, 
		rParent.top + m_iHeight));
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	// Set a timer
	if(m_bOpenAutoHide) SetTimer(ID_TIMEOUT, 1000, (TIMERPROC)TimerProc);

	// Set hooks
	m_pThis = this;
	m_hCallWndHook = ::SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)CallWndProc,
		(HINSTANCE)NULL, (DWORD)::GetCurrentThreadId());
	m_hMouseHook = ::SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, 
		(HINSTANCE)NULL, (DWORD)::GetCurrentThreadId());

	m_lLastAction = ::GetTickCount();

	return TRUE;
}

void CTBar::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// Release hooks
	if(m_hCallWndHook)
	{
		::UnhookWindowsHookEx(m_hCallWndHook);
		m_hCallWndHook = NULL;
	}
	if(m_hMouseHook)
	{
		::UnhookWindowsHookEx(m_hMouseHook);
		m_hMouseHook = NULL;
	}
	m_pThis = NULL;
}

void CTBar::OnPaint() 
{
	// Setup to draw
	CPaintDC dc(this); 
	CRect rClient;
	GetClientRect(&rClient);
	CDC dcOffScreen;
	dcOffScreen.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateDiscardableBitmap(&dc, 
		rClient.Width(), rClient.Height());
	dcOffScreen.SelectObject(bmp);
	CDC dcBmp;
	dcBmp.CreateCompatibleDC(&dc);
	HBITMAP hOldBmp = NULL;

	// split the drawing horz
	CRect rTop = rClient;
	CRect rBottom = rClient;
	rTop.bottom = ((rTop.Height() / 2) + 2);
	rBottom.top = rTop.bottom;	

	// Draw the background
	if(m_bmpBkgndL && m_bmpBkgndC && m_bmpBkgndR)
	{
		hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpBkgndL);
		dcOffScreen.BitBlt(rClient.left, rClient.top, m_bkLSize.cx, m_bkLSize.cy, &dcBmp,0, 0, SRCCOPY);
		dcBmp.SelectObject(m_bmpBkgndC);
		dcOffScreen.StretchBlt(rClient.left+m_bkLSize.cx, rClient.top, rClient.Width()-m_bkLSize.cx-m_bkRSize.cx, m_bkCSize.cy,
			&dcBmp, 0, 0, m_bkCSize.cx, m_bkCSize.cy, SRCCOPY);
		dcBmp.SelectObject(m_bmpBkgndR);
		dcOffScreen.BitBlt(rClient.right-m_bkRSize.cx, rClient.top, m_bkRSize.cx, m_bkRSize.cy, &dcBmp, 0, 0, SRCCOPY);
		dcBmp.SelectObject(hOldBmp);
	}
	else
	{
		DrawGradient(&dcOffScreen, rTop, FALSE, RGB(243,248,252), RGB(198,216,242));
		DrawGradient(&dcOffScreen, rBottom, FALSE, RGB(162,192,231), RGB(103,147,202));
	}

	// Get the mouse position
	POINT point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	CRect rcTitle = rClient;
	rcTitle.left += 6;
	CFont* pFont = dcOffScreen.SelectObject(&m_font);
	int nMode = dcOffScreen.SetBkMode(TRANSPARENT);
	int clrText = dcOffScreen.SetTextColor(m_clrTitle);
	dcOffScreen.DrawText(m_strTitle, rcTitle, DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE);
	dcOffScreen.SetTextColor(clrText);
	dcOffScreen.SetBkMode(nMode);
	dcOffScreen.SelectObject(pFont);
	
	// And window state
	WINDOWPLACEMENT wp = {0};
	GetParent()->GetWindowPlacement(&wp);

	// Get caption button coordinates
	CPoint pos(rClient.right-5, rClient.top+rClient.Height()/2);
	CRect rSpace = rClient;
	int iDiff = ((rSpace.Height() - 16) / 2);
	rSpace.InflateRect(0, -iDiff, -5, -iDiff); 
	// TODO : Move this to function and get bitmap dimensions each time one is changed
	// For now we must assign bitmaps before Create()
	// For now well just count on em bein 16 x 16
	m_rCloseBtn.SetRectEmpty();
	if(m_bShowClose)
	{
		if(m_bmpCloseBtnUp && m_bmpCloseBtnHover && m_bmpCloseBtnDown)
		{
			m_rCloseBtn.top = pos.y - m_closeBtnSize.cy/2;
			m_rCloseBtn.bottom = m_rCloseBtn.top + m_closeBtnSize.cy;
			m_rCloseBtn.right = pos.x;
			m_rCloseBtn.left = m_rCloseBtn.right - m_closeBtnSize.cx;
			pos.x -= m_closeBtnSize.cx+2;
			rSpace.right -= m_closeBtnSize.cx+2;
		}
		else
		{
			m_rCloseBtn = rSpace;
			m_rCloseBtn.left = (m_rCloseBtn.right - 16);
			rSpace.right -= 18;
		}
	}
	m_rMaxBtn.SetRectEmpty();
	if(m_bShowMax)
	{
		if(m_bmpMaxBtnUp && m_bmpMaxBtnHover && m_bmpMaxBtnDown)
		{
			m_rMaxBtn.top = pos.y - m_maxBtnSize.cy/2;
			m_rMaxBtn.bottom = m_rMaxBtn.top + m_maxBtnSize.cy;
			m_rMaxBtn.right = pos.x;
			m_rMaxBtn.left = m_rMaxBtn.right - m_maxBtnSize.cx;
			pos.x -= m_maxBtnSize.cx+2;
			rSpace.right -= m_maxBtnSize.cx+2;
		}
		else
		{
			m_rMaxBtn = rSpace;
			m_rMaxBtn.left = (m_rMaxBtn.right - 16);
			rSpace.right -= 18;
		}
	}
	m_rMinBtn.SetRectEmpty();
	if(m_bShowMin)
	{
		if(m_bmpMinBtnUp && m_bmpMinBtnHover && m_bmpMinBtnDown)
		{
			m_rMinBtn.top = pos.y - m_minBtnSize.cy/2;
			m_rMinBtn.bottom = m_rMinBtn.top + m_minBtnSize.cy;
			m_rMinBtn.right = pos.x;
			m_rMinBtn.left = m_rMinBtn.right - m_minBtnSize.cx;
			pos.x -= m_minBtnSize.cx+2;
			rSpace.right -= m_minBtnSize.cx+2;
		}
		else
		{
			m_rMinBtn = rSpace;
			m_rMinBtn.left = (m_rMinBtn.right - 16);
			rSpace.right -= 18;
		}
	}

	// Draw the close button
	if(m_bShowClose)
	{
		if(m_bmpCloseBtnUp && m_bmpCloseBtnHover && m_bmpCloseBtnDown)
		{
			if(m_rCloseBtn.PtInRect(point))
			{
				if((m_bCloseDown))
					hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpCloseBtnDown);
				else
					hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpCloseBtnHover);
			}
			else hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpCloseBtnUp);
			dcOffScreen.BitBlt(m_rCloseBtn.left, m_rCloseBtn.top, m_closeBtnSize.cx, m_closeBtnSize.cy, &dcBmp,0, 0, SRCCOPY);
			dcBmp.SelectObject(hOldBmp);
		}
		else
		{
			if((m_bCloseDown) && (m_rCloseBtn.PtInRect(point)))
				dcOffScreen.DrawFrameControl(m_rCloseBtn, DFC_CAPTION, DFCS_CAPTIONCLOSE|DFCS_PUSHED); 
			else
				dcOffScreen.DrawFrameControl(m_rCloseBtn, DFC_CAPTION, DFCS_CAPTIONCLOSE); 
		}
	}
	// Draw the maximize or restore button
	if(m_bShowMax)
	{
		if(wp.showCmd == SW_MAXIMIZE)
		{
			if(m_bmpResBtnUp && m_bmpResBtnHover && m_bmpResBtnDown)
			{
				if(m_rMaxBtn.PtInRect(point))
				{
					if((m_bMaxDown))
						hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpResBtnDown);
					else
						hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpResBtnHover);
				}
				else hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpResBtnUp);
				dcOffScreen.BitBlt(m_rMaxBtn.left, m_rMaxBtn.top, m_resBtnSize.cx, m_resBtnSize.cy, &dcBmp,0, 0, SRCCOPY);
				dcBmp.SelectObject(hOldBmp);
			}
			else
			{
				if((m_bMaxDown) && (m_rMaxBtn.PtInRect(point)))
					dcOffScreen.DrawFrameControl(m_rMaxBtn, DFC_CAPTION, DFCS_CAPTIONRESTORE|DFCS_PUSHED); 
				else
					dcOffScreen.DrawFrameControl(m_rMaxBtn, DFC_CAPTION, DFCS_CAPTIONRESTORE );
			}
		}
		else if(wp.showCmd == SW_NORMAL)
		{
			if(m_bmpMaxBtnUp && m_bmpMaxBtnHover && m_bmpMaxBtnDown)
			{
				if(m_rMaxBtn.PtInRect(point))
				{
					if((m_bMaxDown))
						hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpMaxBtnDown);
					else
						hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpMaxBtnHover);
				}
				else hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpMaxBtnUp);
				dcOffScreen.BitBlt(m_rMaxBtn.left, m_rMaxBtn.top, m_maxBtnSize.cx, m_maxBtnSize.cy, &dcBmp,0, 0, SRCCOPY);
				dcBmp.SelectObject(hOldBmp);
			}
			else
			{
				if((m_bMaxDown) && (m_rMaxBtn.PtInRect(point)))
					dcOffScreen.DrawFrameControl(m_rMaxBtn, DFC_CAPTION, DFCS_CAPTIONMAX|DFCS_PUSHED); 
				else
					dcOffScreen.DrawFrameControl(m_rMaxBtn, DFC_CAPTION, DFCS_CAPTIONMAX); 
			}
		}
	}
	// Draw the minimize button
	if(m_bShowMin)
	{
		if(m_bmpMinBtnUp && m_bmpMinBtnHover && m_bmpMinBtnDown)
		{
			if((m_bMinDown) && (m_rMinBtn.PtInRect(point)))
				hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpMinBtnDown);
			else
				hOldBmp = (HBITMAP)dcBmp.SelectObject(m_bmpMinBtnUp);
			dcOffScreen.BitBlt(m_rMinBtn.left, m_rMinBtn.top, m_minBtnSize.cx, m_minBtnSize.cy, &dcBmp,0, 0, SRCCOPY);
			dcBmp.SelectObject(hOldBmp);
		}
		else
		{
			if((m_bMinDown) && (m_rMinBtn.PtInRect(point)))
				dcOffScreen.DrawFrameControl(m_rMinBtn, DFC_CAPTION, DFCS_CAPTIONMIN|DFCS_PUSHED); 
			else
				dcOffScreen.DrawFrameControl(m_rMinBtn, DFC_CAPTION, DFCS_CAPTIONMIN); 
		}
	}

	// Update the screen
	dc.BitBlt(0, 0, rClient.Width(), rClient.Height(), &dcOffScreen, 0, 0, SRCCOPY);
}

void CTBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bLButtonDown = TRUE;

	// Make sure were topmost
	GetParent()->BringWindowToTop();
	GetParent()->UpdateWindow();

	// Check for click on caption buttons
	if(m_rCloseBtn.PtInRect(point))
	{
		m_bCloseDown = TRUE;
		InvalidateRect(m_rCloseBtn);
		SetCapture();
	}
	else if(m_rMaxBtn.PtInRect(point))
	{
		m_bMaxDown = TRUE;
		InvalidateRect(m_rMaxBtn);
		SetCapture();
	}
	else if(m_rMinBtn.PtInRect(point))
	{
		m_bMinDown = TRUE;
		InvalidateRect(m_rMinBtn);
		SetCapture();
	}
	else
	{
		// Make sure were not maximized before dragging
		WINDOWPLACEMENT wp = {0};
		GetParent()->GetWindowPlacement(&wp);
		if(wp.showCmd == SW_MAXIMIZE)
		{
			CWnd::OnLButtonDown(nFlags, point);
			return;
		}

		// Drag the window regularly
		if(!m_bGhostDrag)
		{
			GetParent()->ClientToScreen(&point);
			StartDrag(point);
			return;
		}

		// Ghost drag
		CRect rWnd;
		GetParent()->GetWindowRect(&rWnd);

		// Take a screenshot of ourself
		HBITMAP hBmpDrag = CreateDragImage(&rWnd);

		// Setup image list for dragging
  		if(m_hImageList)
		{
			ImageList_Destroy(m_hImageList);
			m_hImageList = NULL;
		}

		// Create an imagelist and add our screenshot 
		m_hImageList = ImageList_Create(rWnd.Width(), 
			rWnd.Height(), ILC_COLORDDB|ILC_MASK, 0, 1);
		ASSERT(m_hImageList);
		ImageList_AddMasked(m_hImageList, hBmpDrag, RGB(255,255,255));	

		// Lock window update so animations wont bleeed through
		GetParent()->LockWindowUpdate();

		// Dont allow window image to be dragged outside the desktop area 
		POINT ptCursor;
		GetCursorPos(&ptCursor);
		CRect rDesktop = GetDesktopRect();
		rDesktop.top += (ptCursor.y - rWnd.top);
		rDesktop.left += (ptCursor.x - rWnd.left);
		rDesktop.bottom -= (rWnd.bottom - ptCursor.y);
		rDesktop.right -= (rWnd.right - ptCursor.x);
		::ClipCursor(rDesktop);

		// Initialize imagelist drag operation
		GetParent()->ScreenToClient(&ptCursor);
		ImageList_BeginDrag(m_hImageList, 0, ptCursor.x, ptCursor.y);

		// Begin drag operation
		ClientToScreen(&point);
		m_ptOrigin = point;
		ImageList_DragEnter(::GetDesktopWindow(), point.x, point.y);

		// Capture all mouse activity
		SetCapture();
		m_bDragging = TRUE;
		m_ptLast = point;	

		return;
	}

	//zengkairong
	//不屏蔽此语句，在CMainFrame::OnClose()中的CCloseDlg的实例
	//不能响应鼠标消息,该语句会导致调用CTBar::OnDestroy()
	//CWnd::OnLButtonDown(nFlags, point);
}

void CTBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// Release mouse capture
	if(GetCapture() == this)
		ReleaseCapture();
	
	if((m_bGhostDrag) && (m_bDragging))
	{
		GetParent()->UnlockWindowUpdate();
		m_bDragging = FALSE;

		// End the imaglist drag operation
		ImageList_DragLeave(::GetDesktopWindow());
		ImageList_EndDrag();

		// Clean up drag bitmap
		IMAGEINFO ii = {0};
		if(ImageList_GetImageInfo(m_hImageList, 0, &ii))
			::DeleteObject((HBITMAP)ii.hbmImage);
		ImageList_Remove(m_hImageList, 0);

		// Delete imagelist
		ImageList_Destroy(m_hImageList);
		m_hImageList = NULL;

		// Restore cursor movement
		::ClipCursor(NULL);

		// See how far weve moved
		ClientToScreen(&point);
		CPoint ptOffset(point - m_ptOrigin);

		// Move the main window into place
		CRect rWnd;
		GetParent()->GetWindowRect(&rWnd);
		rWnd.OffsetRect(ptOffset);
		GetParent()->MoveWindow(&rWnd);

		return;
	}
	
	// See if were closing
	if((m_bCloseDown) && (m_rCloseBtn.PtInRect(point)))
	{
		m_bCloseDown = FALSE;
		InvalidateRect(m_rCloseBtn);
		GetParent()->PostMessage(WM_CLOSE, 0, 0);
	}
	
	// Check for middle button pressed
	if(m_bMaxDown)
	{
		m_bMaxDown = FALSE;
		InvalidateRect(m_rMaxBtn);
		if(m_rMaxBtn.PtInRect(point))
		{
			WINDOWPLACEMENT wp = {0};
			GetParent()->GetWindowPlacement(&wp);
			if(wp.showCmd == SW_MAXIMIZE)
				GetParent()->ShowWindow(SW_RESTORE);
			else if(wp.showCmd == SW_NORMAL)
				GetParent()->ShowWindow(SW_MAXIMIZE);
		}
	}

	// Check for minimize button pressed
	if((m_bMinDown) && (m_rMinBtn.PtInRect(point)))
	{
		m_bMinDown = FALSE;
		InvalidateRect(m_rMinBtn);
		GetParent()->ShowWindow(SW_MINIMIZE);
	}

	// Reset flags
	m_bLButtonDown = FALSE;
	//m_bDragging = FALSE;
	if(m_bCloseDown)
	{
		m_bCloseDown = FALSE;
		InvalidateRect(m_rCloseBtn);
	}
	else if(m_bMaxDown)
	{
		m_bMaxDown = FALSE;
		InvalidateRect(m_rMaxBtn);
	}
	else if(m_bMinDown)
	{
		m_bMinDown = FALSE;
		InvalidateRect(m_rMinBtn);
	}
	//zengkairong
	//不屏蔽此语句，在CMainFrame::OnClose()中的CCloseDlg的实例
	//不能响应鼠标消息,该语句会导致调用CTBar::OnDestroy()
	//CWnd::OnLButtonUp(nFlags, point); 
}

void CTBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_rCloseBtn.PtInRect(point))
	{
		UpdateHoverItem(CLOSEBTN);
	}
	else if(m_rMaxBtn.PtInRect(point))
	{
		UpdateHoverItem(MAXBTN);
	}
	else if(m_rMinBtn.PtInRect(point))
	{
		UpdateHoverItem(MINBTN);
	}
	else{
		UpdateHoverItem(BKGROUND);
	}

	// Handle ghost drag move
	if((m_bGhostDrag) && (m_bDragging))
	{
		ClientToScreen(&point);
		ImageList_DragMove(point.x, point.y);
		return;
	}
	//zengkairong
	//不屏蔽此语句，在CMainFrame::OnClose()中的CCloseDlg的实例
	//不能响应鼠标消息,该语句会导致调用CTBar::OnDestroy()
	//CWnd::OnMouseMove(nFlags, point);
}

void CTBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Make sure were topmost
	GetParent()->BringWindowToTop();
	GetParent()->UpdateWindow();

	// Maximize / restore main window
	if(m_bShowMax)
	{
		WINDOWPLACEMENT wp = {0};
		GetParent()->GetWindowPlacement(&wp);
		if(wp.showCmd == SW_MAXIMIZE)
			GetParent()->ShowWindow(SW_RESTORE);
		else if(wp.showCmd == SW_NORMAL)
			GetParent()->ShowWindow(SW_MAXIMIZE);
	}
	//zengkairong
	//不屏蔽此语句，在CMainFrame::OnClose()中的CCloseDlg的实例
	//不能响应鼠标消息,该语句会导致调用CTBar::OnDestroy()
	//CWnd::OnLButtonDblClk(nFlags, point);
}

//=================================================
// Property Accessor Functions
//=================================================
BOOL CTBar::SetCloseBtnBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown)
{
	BITMAP bm;
	m_bmpCloseBtnUp = hBmpUp;
	m_bmpCloseBtnHover = hBmpHover;
	m_bmpCloseBtnDown = hBmpDown;
	if(m_bmpCloseBtnUp && m_bmpCloseBtnHover && m_bmpCloseBtnDown
		&& CBitmap::FromHandle(m_bmpCloseBtnUp)->GetBitmap(&bm))
	{
		m_closeBtnSize.SetSize(bm.bmWidth, bm.bmHeight);
	}
	else m_closeBtnSize.SetSize(0,0);
	return TRUE;
}

BOOL CTBar::SetMaxBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown)
{
	BITMAP bm;
	m_bmpMaxBtnUp = hBmpUp;
	m_bmpMaxBtnHover = hBmpHover;
	m_bmpMaxBtnDown = hBmpDown;
	if(m_bmpMaxBtnUp && m_bmpMaxBtnHover && m_bmpMaxBtnDown
		&& CBitmap::FromHandle(m_bmpMaxBtnUp)->GetBitmap(&bm))
	{
		m_maxBtnSize.SetSize(bm.bmWidth, bm.bmHeight);
	}
	else m_maxBtnSize.SetSize(0,0);
	return TRUE;
}

BOOL CTBar::SetResBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown)
{
	BITMAP bm;
	m_bmpResBtnUp = hBmpUp;
	m_bmpResBtnHover = hBmpHover;
	m_bmpResBtnDown = hBmpDown;
	if(m_bmpResBtnUp && m_bmpResBtnHover && m_bmpResBtnDown
		&& CBitmap::FromHandle(m_bmpResBtnUp)->GetBitmap(&bm))
	{
		m_resBtnSize.SetSize(bm.bmWidth, bm.bmHeight);
	}
	else m_resBtnSize.SetSize(0,0);
	return TRUE;
}

BOOL CTBar::SetMinBtnUpBmp(HBITMAP hBmpUp, HBITMAP hBmpHover, HBITMAP hBmpDown)
{
	BITMAP bm;
	m_bmpMinBtnUp = hBmpUp;
	m_bmpMinBtnHover = hBmpHover;
	m_bmpMinBtnDown = hBmpDown;
	if(m_bmpMinBtnUp && m_bmpMinBtnHover && m_bmpMinBtnDown
		&& CBitmap::FromHandle(m_bmpMinBtnUp)->GetBitmap(&bm))
	{
		m_minBtnSize.SetSize(bm.bmWidth, bm.bmHeight);
	}
	else m_minBtnSize.SetSize(0,0);
	return TRUE;
}

BOOL CTBar::SetBackGroundBmp(HBITMAP hBmpL, HBITMAP hBmpC, HBITMAP hBmpR)
{
	BITMAP bmL, bmC, bmR;
	if(hBmpL && hBmpC && hBmpR 
		&& CBitmap::FromHandle(hBmpL)->GetBitmap(&bmL)
		&& CBitmap::FromHandle(hBmpC)->GetBitmap(&bmC)
		&& CBitmap::FromHandle(hBmpR)->GetBitmap(&bmR)
		&& bmL.bmHeight == bmC.bmHeight
		&& bmL.bmHeight == bmR.bmHeight)
	{
		m_bkLSize.SetSize(bmL.bmWidth, bmL.bmHeight);
		m_bkCSize.SetSize(bmC.bmWidth, bmC.bmHeight);
		m_bkRSize.SetSize(bmR.bmWidth, bmR.bmHeight);
		SetBarHeight(bmL.bmHeight);
		m_bmpBkgndL = hBmpL;
		m_bmpBkgndC = hBmpC;
		m_bmpBkgndR = hBmpR;

		if(GetSafeHwnd())
		{
			CRect rParent;
			GetParent()->GetClientRect(&rParent);
			MoveWindow(CRect(rParent.left, rParent.top, 
				rParent.right, rParent.top + m_iHeight));
		}

		return TRUE;
	}
	return FALSE;
}

void CTBar::SetBarHeight(int i)
{
	if(m_bmpBkgndL && m_bmpBkgndC && m_bmpBkgndR)
		return;
	m_iHeight = i;
	if(GetSafeHwnd())
		Invalidate();
}

void CTBar::SetShowClose(BOOL b)
{
	m_bShowClose = b;
	if(GetSafeHwnd())
		Invalidate();
}

void CTBar::SetShowMin(BOOL b)
{
	m_bShowMin = b;
	if(GetSafeHwnd())
		Invalidate();
}

void CTBar::SetShowMax(BOOL b)
{
	m_bShowMax = b;
	if(GetSafeHwnd())
		Invalidate();
}

void CTBar::UpdateHoverItem(int nNewItem)
{
	if(m_nLastHoverItem != nNewItem)
	{
		switch(m_nLastHoverItem) {
		case MINBTN:
			InvalidateRect(m_rMinBtn);
			break;
		case MAXBTN:
			InvalidateRect(m_rMaxBtn);
			break;
		case CLOSEBTN:
			InvalidateRect(m_rCloseBtn);
			break;
		}
		m_nLastHoverItem = nNewItem;
		switch(m_nLastHoverItem) {
		case MINBTN:
			InvalidateRect(m_rMinBtn);
			break;
		case MAXBTN:
			InvalidateRect(m_rMaxBtn);
			break;
		case CLOSEBTN:
			InvalidateRect(m_rCloseBtn);
			break;
		}
	}
}

void CTBar::OpenAutoHide(BOOL bHide /* = TRUE */)
{
	m_bOpenAutoHide = bHide;
	if(GetSafeHwnd())
	{
		if(bHide) KillTimer(ID_TIMEOUT);
		else SetTimer(ID_TIMEOUT, 1000, (TIMERPROC)TimerProc);
	}
}

//=================================================
// Helper Functions
//=================================================
HBITMAP CTBar::CreateDragImage(CRect* pRect, BOOL bAdjustForDesktop/* = TRUE*/)
{
	// Get desktop DC
    HDC hDesktop = ::GetDCEx(::GetDesktopWindow(), 0, DCX_WINDOW);

	if(bAdjustForDesktop)
	{
		// Get width and height of screen
		int cxScreen = ::GetDeviceCaps(hDesktop, HORZRES) + ::GetSystemMetrics(SM_CXBORDER) + 1; 
		int cyScreen = ::GetDeviceCaps(hDesktop, VERTRES) + ::GetSystemMetrics(SM_CYBORDER) + 1; 

		CRect rDesktop = GetDesktopRect();
		
		// Make sure rect is in desktop area
		if(pRect->left < 0)
			pRect->left = 0;
		if(pRect->top < 0)
			pRect->top = 0;
		if(pRect->right > rDesktop.Width())
			pRect->right = rDesktop.Width();
		if(pRect->bottom > rDesktop.Height())
			pRect->bottom = rDesktop.Height();
	}

	// Setup for screen cap
	HDC hDragDC = ::CreateCompatibleDC(hDesktop);
	HBITMAP hBmpDrag = ::CreateCompatibleBitmap(hDesktop, pRect->Width(), pRect->Height());
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDragDC, hBmpDrag);          
  
    // Clear the background
	HBRUSH hBrush = (HBRUSH)::CreateSolidBrush(RGB(0,0,0));
	::FillRect(hDragDC, pRect, hBrush);
    ::DeleteObject(hBrush);

    // Capture an image with the given coordinates
	::BitBlt(hDragDC, 0, 0, pRect->Width(), pRect->Height(), 
		hDesktop, pRect->left, pRect->top, SRCCOPY);
	::SelectObject(hDragDC, hOldBitmap);
  	
	// Clean up
	::DeleteDC(hDragDC);
	::ReleaseDC(::GetDesktopWindow(), hDesktop);

	return hBmpDrag;
}

CRect CTBar::GetDesktopRect()
{
	// Get the desktop rect
	CRect rDesktop = CRect(CPoint(0, 0), 
		CSize(::GetSystemMetrics(SM_CXSCREEN), 
		::GetSystemMetrics(SM_CYSCREEN)));
	rDesktop.InflateRect(-2, -2, 2, 2);

	// See if taskbar's autohide property is on
	APPBARDATA abd = {0};
	abd.cbSize = sizeof(abd);
	abd.hWnd = m_hWnd;
	if(!(::SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE))
	{
		// Get the taskbar rect
		::SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
		CRect rTaskbar = abd.rc;
		
		// Adjust desktop for taskbar
		switch(abd.uEdge)
		{
			case ABE_TOP:
				if(rDesktop.top < rTaskbar.bottom)
					rDesktop.top = rTaskbar.bottom;
				break;
			case ABE_BOTTOM:
				if(rDesktop.bottom > rTaskbar.top)
					rDesktop.bottom = rTaskbar.top;
				break;
			case ABE_LEFT:
				if(rDesktop.left < rTaskbar.right)
					rDesktop.left = rTaskbar.right;
				break;
			case ABE_RIGHT:
				if(rDesktop.right > rTaskbar.left)
					rDesktop.right = rTaskbar.left;
				break;
		}
	}
	return rDesktop;
}

void CTBar::Autohide(BOOL bHide/*=TRUE*/)
{
	//TRACE1(_T("Autohide:%d\n"), bHide);
	if(bHide == m_bHidden)
		return;

	CRect rc;
	
	if(bHide)
	{
		CRect rParent;
		GetParent()->GetClientRect(&rParent);
		rc = CRect((rParent.left + 2), rParent.top - m_iHeight, 
			(rParent.right - 2), rParent.top);
	}
	else
	{	
		CRect rParent;
		GetParent()->GetClientRect(&rParent);
		rc = CRect((rParent.left + 2), rParent.top, 
			(rParent.right - 2), (rParent.top + m_iHeight));
	}

	// Slide window in from or out to the edge
	SlideWindow(rc);

	// Save state
	m_bHidden = bHide;

	// Show/hide the cursor
	::ShowCursor(!bHide);
}

void CTBar::SlideWindow(CRect& rEnd)
{
	CRect rStart;
	GetWindowRect(&rStart);   
	GetParent()->ScreenToClient(&rStart);

	int iTime = 200;
	if(rStart != rEnd)
	{
		// Get our starting and ending time.
		DWORD dwTimeStart = GetTickCount();
		DWORD dwTimeEnd = dwTimeStart + iTime;
		DWORD dwTime;
		BOOL bUp = (rEnd.bottom < rStart.bottom);
		int iBottom = rStart.bottom;
		
		while((dwTime = ::GetTickCount()) < dwTimeEnd)
		{
			// While we are still sliding, calculate our new position
			int x = rStart.left - (rStart.left - rEnd.left) 
				* (int)(dwTime - dwTimeStart) / iTime;

			int y = rStart.top  - (rStart.top  - rEnd.top)  
				* (int)(dwTime - dwTimeStart) / iTime;

			int iWidth  = rStart.Width()  - (rStart.Width()  - rEnd.Width())  
				* (int)(dwTime - dwTimeStart) / iTime;

			int iHeight = rStart.Height() - (rStart.Height() - rEnd.Height()) 
				* (int)(dwTime - dwTimeStart) / iTime;

			// Show the window at its changed position
			SetWindowPos(NULL/*&CWnd::wndTopMost*/, x, y, iWidth, iHeight, 
				SWP_NOZORDER|SWP_NOACTIVATE|SWP_DRAWFRAME);
			UpdateWindow();
			
			if(bUp)
			{
				CRect rUpdate(rStart.left, (iBottom - iHeight), rStart.right, iBottom);
				GetParent()->ScreenToClient(rUpdate);
				GetParent()->InvalidateRect(rUpdate);
				iBottom = rUpdate.top;
			}
		}
	}

	// Make sure that the window is at its final position
	MoveWindow(rEnd.left, rEnd.top, rEnd.Width(), rEnd.Height());
	Invalidate();
}

//=================================================
// Hook Callbacks
//=================================================
LRESULT CALLBACK CTBar::CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0)
		return CallNextHookEx(m_hCallWndHook, nCode, wParam, lParam);
	if (!m_pThis)
		return CallNextHookEx(m_hCallWndHook, nCode, wParam, lParam);

	CWPSTRUCT* pCW = (CWPSTRUCT*)lParam;
	ASSERT(pCW);

	if(pCW->hwnd == m_pThis->GetParent()->GetSafeHwnd()/*AfxGetMainWnd()->m_hWnd*/)
	{
		switch(pCW->message)
		{
			case WM_WINDOWPOSCHANGED:
			case WM_SHOWWINDOW:
			case WM_SIZE:
			{
				// Position the window
				CRect rParent;
				m_pThis->GetParent()->GetClientRect(&rParent);
				m_pThis->MoveWindow(CRect(rParent.left, rParent.top, 
					rParent.right, rParent.top + m_pThis->m_iHeight));
				break;
			}
		}
	}

	return ::CallNextHookEx(m_hCallWndHook, nCode, wParam, lParam);
}

LRESULT CALLBACK CTBar::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode < 0)
		return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
	if (!m_pThis)
		return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);

	MOUSEHOOKSTRUCT* pMS = (MOUSEHOOKSTRUCT*)lParam;
	ASSERT(pMS);

	// Get local info
	CRect rClient;
	m_pThis->GetClientRect(&rClient);
	CPoint pt(pMS->pt);
	m_pThis->ScreenToClient(&pt);
	BOOL bOver = rClient.PtInRect(pt);

	if((m_pThis->m_ptLastHook.x != pMS->pt.x) && (m_pThis->m_ptLastHook.y != pMS->pt.y))
	{
		if(wParam == WM_MOUSEMOVE)
		{
			// If were hidden then come out
			if(m_pThis->m_bHidden)
				m_pThis->Autohide(FALSE);

			// Keep track for autohide on idle
			m_pThis->m_lLastAction = ::GetTickCount();
		}
	}
	m_pThis->m_ptLastHook = pMS->pt;

	// Pass the message on if it concerns us
	if((wParam == WM_LBUTTONDOWN) && (bOver))
	{
		m_pThis->OnLButtonDown(MK_LBUTTON, pt);
		return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
	}
	
	return ::CallNextHookEx(m_hMouseHook, nCode, wParam, lParam);
}

//=================================================
// Timer Callback
//=================================================
void CALLBACK CTBar::TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if (!m_pThis) 
		return;

	if(!m_pThis->m_bAutohide)
		return;

	// Reset count if any button is down
	if(m_pThis->m_bLButtonDown)
		m_pThis->m_lLastAction = ::GetTickCount();

	long lSeconds = ((dwTime - m_pThis->m_lLastAction) / 1000);

	if((int)(dwTime - m_pThis->m_lLastAction) > m_pThis->m_iTimeout)
	{
		if(!m_pThis->m_bHidden)
		{
			m_pThis->Autohide();
		}
	}
}

//=================================================
// Helper Functions
//=================================================
void CTBar::DrawGradient(CDC* pDC, CRect rect, BOOL bHorz, COLORREF clrStart, COLORREF clrEnd) 
{
	// Draw the gradient
	RECT rectFill;		// Rectangle for filling band
	float fStep;        // How wide is each band?
	CBrush brush;		// Brush to fill in the bar	

	// First find out the largest color distance between the start and end colors.  This distance
	// will determine how many steps we use to carve up the client region and the size of each
	// gradient rect.
	int r, g, b;							// First distance, then starting value
	float rStep, gStep, bStep;		// Step size for each color

	// Get the color differences
	r = (GetRValue(clrEnd) - GetRValue(clrStart));
	g = (GetGValue(clrEnd) - GetGValue(clrStart));
	b = (GetBValue(clrEnd) - GetBValue(clrStart));

	// Make the number of steps equal to the greatest distance
	int nSteps = max(abs(r), max(abs(g), abs(b)));

	if(bHorz)
	{

		// Determine how large each band should be in order to cover the
		// client with nSteps bands (one for every color intensity level)
		fStep = (float)rect.right / (float)nSteps;

		// Calculate the step size for each color
		rStep = r/(float)nSteps;
		gStep = g/(float)nSteps;
		bStep = b/(float)nSteps;

		// Reset the colors to the starting position
		r = GetRValue(clrStart);
		g = GetGValue(clrStart);
		b = GetBValue(clrStart);

		// Start filling bands
		for(int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
		{
			
			::SetRect(&rectFill,
						(int)(iOnBand * fStep),     // Upper left X
						 0,							// Upper left Y
						(int)((iOnBand+1) * fStep), // Lower right X
						rect.bottom+1);			// Lower right Y
		
			// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
			VERIFY(brush.CreateSolidBrush(
				RGB(r+rStep*iOnBand, g + gStep*iOnBand, b + bStep * iOnBand)));
			pDC->FillRect(&rectFill,&brush);
			VERIFY(brush.DeleteObject());

			if(rectFill.right > rect.right)
				break;
		}
	}
	else
	{
		// Determine how large each band should be in order to cover the
		// client with nSteps bands (one for every color intensity level)
		fStep = (float)rect.Height() / (float)nSteps;

		// Calculate the step size for each color
		rStep = r/(float)nSteps;
		gStep = g/(float)nSteps;
		bStep = b/(float)nSteps;

		// Reset the colors to the starting position
		r = GetRValue(clrStart);
		g = GetGValue(clrStart);
		b = GetBValue(clrStart);

		// Start filling bands
		for(int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
		{
			::SetRect(&rectFill, rect.left,	rect.top + (int)(iOnBand * fStep),
				rect.right+1, rect.top + (int)((iOnBand+1) * fStep));			
		
			// CDC::FillSolidRect is faster, but it does not handle 8-bit color depth
			VERIFY(brush.CreateSolidBrush(
				RGB(r+rStep*iOnBand, g + gStep*iOnBand, b + bStep * iOnBand)));
			pDC->FillRect(&rectFill,&brush);
			VERIFY(brush.DeleteObject());

			if(rectFill.bottom > rect.bottom)
				break;
		}
	}	
}

//=================================================
// Dragging Helper Functions
//=================================================
void CTBar::StartDrag(CPoint pt) 
{
	if(m_bDragging) return;
	m_bDragging	= TRUE;
	m_ptLast	= pt;

	// Handle pending WM_PAINT messages
	MSG msg;
	while(::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_NOREMOVE))
	{
		if(!GetMessage(&msg, NULL, WM_PAINT, WM_PAINT))
			return;
		DispatchMessage(&msg);
	}

	Track();
}

void CTBar::Move(CPoint pt)
{
	// Move the window to the new position
	CPoint ptOffset = (pt - m_ptLast);
	if(ptOffset != m_ptLast)
	{
		CRect rThis;
		GetParent()->GetWindowRect(&rThis);
		rThis.OffsetRect(ptOffset);
		GetParent()->MoveWindow(&rThis);
	}

	// Save the last mouse position
	m_ptLast = pt;
}

void CTBar::EndDrag()
{
	ReleaseCapture();
	m_bDragging = FALSE;
}

BOOL CTBar::Track()
{
	// Don't handle if capture already set
	if(::GetCapture() != NULL)
		return FALSE;

	// Set capture to the window which received this message
	SetCapture();
	ASSERT(::GetCapture() == m_hWnd);

	// Get messages until capture lost or cancelled/accepted
	while(::GetCapture() == m_hWnd)
	{
		MSG msg;
		if(!::GetMessage(&msg, NULL, 0, 0))
		{
			::PostQuitMessage(msg.wParam);
			break;
		}

		switch(msg.message)
		{
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_LBUTTONUP:
				if(m_bDragging)
					EndDrag();
				return TRUE;
			case WM_MOUSEMOVE:
				if(m_bDragging)
				{
					if(GetKeyState(VK_LBUTTON) & 0x8000)
						Move(msg.pt);
					else m_bDragging = FALSE;
				}
				break;
			case WM_KEYDOWN:
				if(msg.wParam == VK_ESCAPE)
				{
					EndDrag();
					return FALSE;
				}
				break;
			case WM_LBUTTONDBLCLK:
				OnLButtonDblClk((UINT)msg.wParam, msg.pt);
				return TRUE;
			default:
				DispatchMessage(&msg);
				break;
		}
	}
	EndDrag();

	return FALSE;
}

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   