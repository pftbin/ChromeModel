// SkinDlg.cpp : 实现文件
//

#include "pch.h"
#include "SkinDlg.h"
#include ".\skindlg.h"


// CSkinDlg 对话框

IMPLEMENT_DYNAMIC(CSkinDlg, CDialog)
CSkinDlg::CSkinDlg(int nIDD, CWnd* pParent /*=NULL*/)
	: CDialog(nIDD, pParent)
{
	m_crBack = RGB(72, 72, 72);
	m_hBmpBkgnd = NULL;
	memset(&m_bmBkgnd, 0, sizeof(m_bmBkgnd));
}

CSkinDlg::~CSkinDlg()
{
}

void CSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CSkinDlg::SetBkgndBmp(HBITMAP hBmp)
{
	if(hBmp == NULL) return;
	m_hBmpBkgnd = hBmp;
	CBitmap::FromHandle(hBmp)->GetBitmap(&m_bmBkgnd);
}

void CSkinDlg::SetBkgndColor(COLORREF color)
{
	m_crBack = color;
	if(GetSafeHwnd()) Invalidate();
}


BEGIN_MESSAGE_MAP(CSkinDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSkinDlg 消息处理程序

BOOL CSkinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TOOLWINDOW);
	LONG lStyle = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
	::SetWindowLong(this->m_hWnd, GWL_STYLE, lStyle & ~WS_CAPTION);
	::SetWindowPos(this->m_hWnd, NULL, 0, 0, 0, 0,
		SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	if(!m_tBar.Create(this, 0))
	{
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CSkinDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient, rect;
	if(m_tBar.GetSafeHwnd())
	{
		m_tBar.GetWindowRect(&rect);
		ScreenToClient(&rect);
		pDC->ExcludeClipRect(&rect);
		rcClient.top = rect.bottom;
	}

	GetClientRect(&rcClient);
	rect = rcClient;
	int cxBorder = GetSystemMetrics(SM_CXDLGFRAME);
	int cyBorder = GetSystemMetrics(SM_CYDLGFRAME);

	if(m_hBmpBkgnd)
	{
		CDC dcBmp; dcBmp.CreateCompatibleDC(pDC);
		HBITMAP hOldBmp = (HBITMAP)dcBmp.SelectObject(m_hBmpBkgnd);
		pDC->BitBlt(rcClient.left, rcClient.bottom-cyBorder, cxBorder, cyBorder,
			&dcBmp, 0, m_bmBkgnd.bmHeight-cyBorder, SRCCOPY);
		pDC->BitBlt(rcClient.right-cxBorder, rcClient.bottom-cyBorder, cxBorder, cyBorder,
			&dcBmp, m_bmBkgnd.bmWidth-cxBorder, m_bmBkgnd.bmHeight-cyBorder, SRCCOPY);
		pDC->StretchBlt(rcClient.left, rcClient.top, cxBorder, rcClient.Height()-cyBorder,
			&dcBmp, 0, 0, cxBorder, 1, SRCCOPY);
		pDC->StretchBlt(rcClient.right-cxBorder, rcClient.top, cxBorder, rcClient.Height()-cyBorder,
			&dcBmp, m_bmBkgnd.bmWidth-cxBorder, 0, cxBorder, 1, SRCCOPY);
		pDC->StretchBlt(rcClient.left+cxBorder, rcClient.bottom-cyBorder, rcClient.Width()-2*cxBorder, cyBorder,
			&dcBmp, m_bmBkgnd.bmWidth/2, m_bmBkgnd.bmHeight-cyBorder, 1, cyBorder, SRCCOPY);
		dcBmp.SelectObject(hOldBmp);

		rect.left += cxBorder;
		rect.right -= cxBorder;
		rect.bottom -= cyBorder;
	}

	pDC->FillSolidRect(&rect, m_crBack);

	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}

void CSkinDlg::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	CDialog::OnNcCalcSize(bCalcValidRects, lpncsp);

	int cx = GetSystemMetrics(SM_CXDLGFRAME);
	int cy = GetSystemMetrics(SM_CYDLGFRAME);
	lpncsp->rgrc[0].left -= cx;
	lpncsp->rgrc[0].right += cx;
	lpncsp->rgrc[0].top -= cy;
	lpncsp->rgrc[0].bottom += cy;
}

HBRUSH CSkinDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if(nCtlColor == CTLCOLOR_STATIC)
	{
		DWORD dwStyle = pWnd->GetStyle();
		if(!(dwStyle&ES_READONLY))
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			hbr = (HBRUSH)::GetStockObject(NULL_BRUSH);
		}
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
