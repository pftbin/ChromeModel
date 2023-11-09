// HoverButton.cpp : implementation file
//

#include "pch.h"
#include "HoverButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoverButton

CHoverButton::CHoverButton()
{
	m_bHover = FALSE;
	m_bTracking = FALSE;
	m_dcBk.m_hDC = NULL;
	m_bRedraw=TRUE;
	m_bPress=FALSE;
	m_bCheckButton=FALSE;
	m_hBmp = NULL;
}

CHoverButton::~CHoverButton()
{
	DeleteObject(m_dcBk.SelectObject(&m_bmpBkOld));
	DeleteDC(m_dcBk);	
}

void CHoverButton::PreSubclassWindow() 
{
	UINT nBS = GetButtonStyle();

	// Switch to owner-draw
	ModifyStyle(SS_TYPEMASK, BS_OWNERDRAW, SWP_FRAMECHANGED);

	CButton::PreSubclassWindow();
}

IMPLEMENT_DYNAMIC(CHoverButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CHoverButton, CBitmapButton)
	//{{AFX_MSG_MAP(CHoverButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, &OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, &OnMouseHover)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////
 //	CHoverButton message handlers
		
void CHoverButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	//	TODO: Add your message handler code here and/or call default

	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}
	CBitmapButton::OnMouseMove(nFlags, point);
}

BOOL CHoverButton::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);		
	return CButton::PreTranslateMessage(pMsg);
}

// Set the tooltip with a string resource
void CHoverButton::SetToolTipText(int nId, BOOL bActivate)
{
	CString sText;

	// load string resource
	sText.LoadString(nId);
	// If string resource is not empty
	if (sText.IsEmpty() == FALSE) SetToolTipText(&sText, bActivate);

}

// Set the tooltip with a CString
void CHoverButton::SetToolTipText(CString *spText, BOOL bActivate)
{
	// We cannot accept NULL pointer
	if (spText == NULL) return;

	// Initialize ToolTip
	InitToolTip();

	// If there is no tooltip defined then add it
	if (m_ToolTip.GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool(this, (LPCTSTR)*spText, rectBtn, 1);
	}

	// Set text for tooltip
	m_ToolTip.UpdateTipText((LPCTSTR)*spText, this, 1);
	m_ToolTip.Activate(bActivate);
}

void CHoverButton::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

// Activate the tooltip
void CHoverButton::ActivateTooltip(BOOL bActivate)
{
	// If there is no tooltip then do nothing
	if (m_ToolTip.GetToolCount() == 0) return;

	// Activate tooltip
	m_ToolTip.Activate(bActivate);
} // End of EnableTooltip


void CHoverButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item

	CDC *mydc=CDC::FromHandle(lpDrawItemStruct->hDC);

/*	CDC * pMemDC = new CDC;
	pMemDC -> CreateCompatibleDC(mydc);



	CBitmap * pOldBitmap;
	pOldBitmap = pMemDC -> SelectObject(&mybitmap);*/
	CClientDC clientDC(GetParent());
	CRect crect;
	CRect wrect;
	GetClientRect(crect);
	GetWindowRect(wrect);
	GetParent()->ScreenToClient(wrect);
	if (m_bRedraw)
	{
		if(m_dcBk.m_hDC)
		{
			m_dcBk.DeleteDC();
			m_dcBk.m_hDC=NULL;
			m_bmpBk.DeleteObject();
		}
		m_dcBk.CreateCompatibleDC(&clientDC);
		m_bmpBk.CreateCompatibleBitmap(&clientDC, crect.Width(), crect.Height());
		m_bmpBkOld = m_dcBk.SelectObject(&m_bmpBk);
		m_dcBk.BitBlt(0, 0, crect.Width(), crect.Height(), &clientDC, wrect.left, wrect.top, SRCCOPY);
		m_bRedraw=FALSE;
	}
	mydc->BitBlt(0, 0, crect.Width(), crect.Height(), &m_dcBk, 0, 0, SRCCOPY);
//	mydc->SetBkMode(TRANSPARENT);
//	mydc->SelectStockObject(NULL_BRUSH);
	CPoint point(0,0);	
		
	if(lpDrawItemStruct->itemState & ODS_SELECTED || m_bPress)
	{
		//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx,0,SRCCOPY);
		m_ImageList.Draw(mydc,2,point,ILD_NORMAL);

		if(m_bCheckButton && lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			if(!m_bPress)m_bPress=TRUE;else m_bPress=FALSE;
		}
	}
	else if(lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		m_ImageList.Draw(mydc,3,point,ILD_NORMAL);
	}
	else
	{
		if(m_bHover)
		{
			m_ImageList.Draw(mydc,1,point,ILD_NORMAL);
			//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,m_ButtonSize.cx*2,0,SRCCOPY);
		}else
		{
			m_ImageList.Draw(mydc,0,point,ILD_NORMAL);
			//mydc->BitBlt(0,0,m_ButtonSize.cx,m_ButtonSize.cy,pMemDC,0,0,SRCCOPY);
		}	
	}

	// clean up
//	pMemDC -> SelectObject(pOldBitmap);
//	delete pMemDC;
}

// Load a bitmap from the resources in the button, the bitmap has to have 3 buttonsstates next to each other: Up/Down/Hover
BOOL CHoverButton::LoadBitmap(UINT bitmapid,int cx,COLORREF cMask,BOOL bMask)
{
	CBitmap bmp;
	HINSTANCE hInst = ::AfxGetInstanceHandle();
	bmp.Attach(::LoadImage(::AfxFindResourceHandle(MAKEINTRESOURCE(bitmapid), RT_BITMAP),MAKEINTRESOURCE(bitmapid), IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS));
	BITMAP	bitmapbits;
	bmp.GetBitmap(&bitmapbits);
	m_ButtonSize.cy=bitmapbits.bmHeight;
	m_ButtonSize.cx=bitmapbits.bmWidth/4;
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER);
	UINT flags=ILC_COLOR24;
	if(bMask)flags|=ILC_MASK;
	m_ImageList.DeleteImageList();
	m_ImageList.Create(m_ButtonSize.cx,m_ButtonSize.cy,flags,1,0);
	m_ImageList.Add(&bmp,cMask);
	Invalidate();
	return TRUE;
}

BOOL CHoverButton::SetBitmap(HBITMAP hBmp, COLORREF cMask, BOOL bMask)
{
	if(hBmp == NULL) return FALSE;
	m_hBmp = hBmp;
	CBitmap* pBmp = CBitmap::FromHandle(hBmp);
	BITMAP	bitmapbits;
	pBmp->GetBitmap(&bitmapbits);
	m_ButtonSize.cy=bitmapbits.bmHeight;
	m_ButtonSize.cx=bitmapbits.bmWidth/4;
	SetWindowPos( NULL, 0,0, m_ButtonSize.cx,m_ButtonSize.cy,SWP_NOMOVE   |SWP_NOOWNERZORDER);
	UINT flags=ILC_COLOR24;
	if(bMask)flags|=ILC_MASK;
	m_ImageList.DeleteImageList();
	m_ImageList.Create(m_ButtonSize.cx,m_ButtonSize.cy,flags,1,0);
	m_ImageList.Add(pBmp,cMask);
	Invalidate();
	return TRUE;
}

LRESULT CHoverButton::OnMouseHover(WPARAM wparam, LPARAM lparam) 
{
	// TODO: Add your message handler code here and/or call default
	m_bHover=TRUE;
	Invalidate();
	return S_OK;
}


LRESULT CHoverButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHover=FALSE;
	Invalidate();
	return 0;
}

void CHoverButton::RedrawButton()
{
	m_bRedraw=TRUE;
	Invalidate();
}

BOOL CHoverButton::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CHoverButton::SetPressed(BOOL bPress)
{
	m_bPress=bPress;
	Invalidate();
}

void CHoverButton::SetCheckButton(BOOL bSet)
{
	m_bCheckButton=bSet;
}

