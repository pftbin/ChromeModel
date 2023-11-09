// AirMsgBox.cpp : 实现文件
//

#include "pch.h"
#include "AirMsgBox.h"

#define  LOADBMP_FROM_INI	0

// CAirMsgBox 对话框

IMPLEMENT_DYNAMIC(CAirMsgBox, CSkinDlg)

#define BUTTONWIDTH    46   //按钮默认宽度
#define BUTTONHEIGHT   20   //按钮默认高度
#define FONTHEIGHT     14   //字体默认高度
#define MINCOUNTPERROW 16   //每行最少显示字的个数
#define DLGWIDTH       230  //默认窗口宽度
#define DLGHEITH       140  //默认窗口高度
#define INITSHOWCOUNT  30
#define REDUNDANCYROW  6


CAirMsgBox::CAirMsgBox(CWnd* pParent /*=NULL*/)
: CSkinDlg(CAirMsgBox::IDD, pParent)
{
	int iH = 12;
	//int nW = FW_NORMAL + i * 25;
	m_pftYaheipx = new CFont();
	m_pftYaheipx->CreateFont(
		-iH,                       // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL/*nW*/,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("微软雅黑"));

}

CAirMsgBox::~CAirMsgBox()
{
	if(m_pftYaheipx)
	{
		m_pftYaheipx->DeleteObject();
		delete m_pftYaheipx;
		m_pftYaheipx = NULL;
	}
}

void CAirMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CSkinDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MSG, m_stcMsg);
}

CString GetAppFolder()
{
	TCHAR exeFullName[MAX_PATH];
	::GetModuleFileName(NULL,exeFullName,MAX_PATH);
	CString mPath=exeFullName;

	int mPos=mPath.ReverseFind(_T('\\'));  

	if(mPos==-1)return _T("");	//没有发现

	mPath=mPath.Left(mPos);

	if(mPath.Right(1)==_T("\\"))
	{
		mPath=mPath.Left(mPos-1);
	}
	return mPath;
}

BEGIN_MESSAGE_MAP(CAirMsgBox, CSkinDlg)
END_MESSAGE_MAP()

BOOL CAirMsgBox::OnInitDialog()
{
	CSkinDlg::OnInitDialog ();
	ModifyStyle(WS_CAPTION | WS_THICKFRAME, WS_MAXIMIZEBOX);

	//////////////////////////////////////////////////////////////
	CSize size;
	HBITMAP hBmp, hBmp1, hBmp2, hBmp3, hBmp4, hBmp5, hBmp6;
#if LOADBMP_FROM_INI

	hBmp = CSkinMgr::Instance()->LoadBitmap(_T("DlgBk"), size);
	SetBkgndBmp(hBmp);

	hBmp1 = CSkinMgr::Instance()->LoadBitmap(_T("CloseUp"), size);
	hBmp2 = CSkinMgr::Instance()->LoadBitmap(_T("CloseHover"), size);
	hBmp3 = CSkinMgr::Instance()->LoadBitmap(_T("CloseDown"), size);
	m_tBar.SetCloseBtnBmp(hBmp1, hBmp2, hBmp3);
	m_tBar.SetShowMin(FALSE);

	hBmp4 = CSkinMgr::Instance()->LoadBitmap(_T("TitleBarBkL"), size);
	hBmp5 = CSkinMgr::Instance()->LoadBitmap(_T("TitleBarBkC"), size);
	hBmp6 = CSkinMgr::Instance()->LoadBitmap(_T("TitleBarBkR"), size);
	m_tBar.SetBackGroundBmp(hBmp4, hBmp5, hBmp6);
	
#else

	//皮肤
	m_BitmapDialogBk.LoadBitmap(IDB_BITMAP_DLGBK);
	hBmp = (HBITMAP)m_BitmapDialogBk.m_hObject;
	SetBkgndBmp(hBmp);

	m_BitmapCloseUp.LoadBitmap(IDB_BITMAP_UP);
	m_BitmapCloseHover.LoadBitmap(IDB_BITMAP_HOVER);
	m_BitmapCloseDown.LoadBitmap(IDB_BITMAP_DOWN);
	hBmp1 = (HBITMAP)m_BitmapCloseUp.m_hObject;
	hBmp2 = (HBITMAP)m_BitmapCloseHover.m_hObject;
	hBmp3 = (HBITMAP)m_BitmapCloseDown.m_hObject;
	m_tBar.SetCloseBtnBmp(hBmp1, hBmp2, hBmp3);
	m_tBar.SetShowMin(FALSE);

	m_BitmapTitleBarL.LoadBitmap(IDB_BITMAP_TBL);
	m_BitmapTitleBarC.LoadBitmap(IDB_BITMAP_TBC);
	m_BitmapTitleBarR.LoadBitmap(IDB_BITMAP_TBR);
	hBmp4 = (HBITMAP)m_BitmapTitleBarL.m_hObject;
	hBmp5 = (HBITMAP)m_BitmapTitleBarC.m_hObject;
	hBmp6 = (HBITMAP)m_BitmapTitleBarR.m_hObject;
	m_tBar.SetBackGroundBmp(hBmp4, hBmp5, hBmp6);

#endif

	/////////////////////////////////////////////////////////
	m_strTitle.Trim(_T(" "));
	m_strTitle.Trim(_T(""));
	int n = m_strTitle.GetLength();
	CString strT1;
	for (int i = 0; i < n; i++)
	{
		TCHAR ch = m_strTitle[i];
		if(ch != _T(' '))
			strT1 += m_strTitle[i];
	}
	m_strTitle = strT1;
	m_tBar.SetTitle(m_strTitle, RGB(255,255,255));

	if(m_strTitle.IsEmpty ())
		m_strTitle = _T("提示");
	m_pStaPic = (CStatic*)(GetDlgItem (IDC_STATIC_PIC));
	m_stcMsg.SetFont(m_pftYaheipx, TRUE);

	FormatMsgContont();
	SetPicture();
	AdjustDlgPosition();
	return TRUE;
}

CRect m_szBorder = CRect(8, 32, 8, 8);
void CAirMsgBox::AdjustDlgPosition()
{
	CRect rcIcon, rcText(0, 0, 0, 0), rcBtn, rcBg;

	CDC* pDC = m_stcMsg.GetDC();

	int nOldMode = pDC->SetBkMode(TRANSPARENT);
	CFont *pftOld = pDC->SelectObject(m_pftYaheipx);
	pDC->DrawText(m_strMsg, &rcText, DT_CALCRECT | DT_NOPREFIX);
	pDC->SetBkMode(nOldMode);
	pDC->SelectObject(pftOld);
	m_stcMsg.ReleaseDC(pDC);

	m_pStaPic->GetClientRect(&rcIcon);
	m_stcMsg.SetWindowText(m_strMsg);

	///-------------------------------------// 66×29(pixels)
	long lTop = m_szBorder.top + 12;
	long lRight = 0, btnW = 66, btnH = 29;
	long lWidth = 12 + rcText.Width() + 5 + rcIcon.Width() + 12;
	long lHight = lTop + max(rcText.Height(), rcIcon.Height()) + 10 + btnH + 12 + m_szBorder.bottom + 12;

	m_pStaPic->MoveWindow(15, lTop, rcIcon.Width(), rcIcon.Height());
	int nOffH = (rcIcon.Height() - rcText.Height()) / 2;
	if(nOffH < 0) nOffH = 0;
	m_stcMsg.MoveWindow(12 + rcIcon.Width() + 10, lTop + nOffH, rcText.Width(), rcText.Height());

	rcBtn.top = lTop + max(rcText.Height(), rcIcon.Height()) + 12 + 12;
	rcBtn.bottom = rcBtn.top + btnH;

	rcBg.top = rcBtn.top - 12;
	rcBg.left = 0;
	rcBg.bottom = lHight;

	BOOL bCenter = TRUE;

	//SetButton();
	CSize size;
	HBITMAP hBitmapOK, hBitmapCancel;
#if LOADBMP_FROM_INI

	hBitmapOK = CSkinMgr::Instance()->LoadBitmap(_T("Okex"), size);
	hBitmapCancel = CSkinMgr::Instance()->LoadBitmap(_T("Cancelex"), size);

#else
	
	m_BitmapOK.LoadBitmap(IDB_BITMAP_OKEX);
	hBitmapOK = (HBITMAP)m_BitmapOK.m_hObject;
	m_BitmapCancel.LoadBitmap(IDB_BITMAP_CANCELEX);
	hBitmapCancel = (HBITMAP)m_BitmapCancel.m_hObject;
#endif

	switch ((m_uID & 0x0000000f))
	{
	case MB_YESNO:
	case MB_OKCANCEL:
		{
			lRight = max(15 + btnW + 10 + btnW + 15, lWidth) + m_szBorder.left * 2;
			rcBg.right = lRight;

			if (bCenter)
			{
				rcBtn.left = (lRight - 2 * btnW - 10) / 2;
				rcBtn.right = rcBtn.left + btnW;
				//m_Btn1.MoveWindow(rcBtn);
				m_btnA.Create(_T("OK"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcBtn, this, IDOK);
				m_btnA.SetBitmap (hBitmapOK, RGB(255,0,255), FALSE);

				rcBtn.OffsetRect(btnW + 10, 0);
				//m_Btn3.MoveWindow(rcBtn);
				m_btnB.Create(_T("CANCEL"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcBtn, this, IDCANCEL);
				m_btnB.SetBitmap (hBitmapCancel, RGB(255,0,255), FALSE);
			}
			else
			{
				rcBtn.right = lRight - m_szBorder.right - 12;
				rcBtn.left = rcBtn.right - btnW;

				//m_Btn3.MoveWindow(rcBtn);
				m_btnB.Create(_T("CANCEL"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcBtn, this, IDCANCEL);
				m_btnB.SetBitmap (hBitmapCancel, RGB(255,0,255), FALSE);

				rcBtn.OffsetRect(-(btnW + 10), 0);
				//m_Btn1.MoveWindow(rcBtn);
				m_btnA.Create(_T("OK"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcBtn, this, IDOK);
				m_btnA.SetBitmap (hBitmapOK, RGB(255,0,255), FALSE);
			}
			break;
		}
	default:
		{
			lRight = max(15 + btnW + 15, lWidth) + m_szBorder.left * 2;
			rcBg.right = lRight;

	///*		if (bCenter)
	//		{*/
				rcBtn.left = (lRight - btnW) / 2;
				rcBtn.right = rcBtn.left + btnW;
		/*	}
			else
			{
				rcBtn.right = lRight - m_szBorder.right - 12;
				rcBtn.left = rcBtn.right - btnW;
			}*/

			m_btnA.Create(_T("OK"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcBtn, this, IDOK);
			m_btnA.SetBitmap (hBitmapOK, RGB(255,0,255), FALSE);

			break;
		}// default
	}// switch

	static int nScreenX = ::GetSystemMetrics(SM_CXSCREEN);
	static int nScreenY = ::GetSystemMetrics(SM_CYSCREEN);

	if ((nScreenX <= nScreenY * 2)) // single display
	{
		SetWindowPos(NULL, 0, 0, lRight, lHight, SWP_NOZORDER | SWP_NOMOVE);
		CenterWindow(AfxGetMainWnd());
		return;
	}
	else // multi
	{
		int iOffset = 0;
		CWnd * pWnd = GetParent();
		if (NULL != pWnd)
		{
			CRect rcParent;
			pWnd->GetWindowRect(&rcParent);
			if (rcParent.left > nScreenX / 2) iOffset = nScreenX / 2;
		}
		else
		{
			pWnd = AfxGetMainWnd();
		}
		nScreenX = nScreenX / 4 - lRight / 2 + iOffset;
		nScreenY = nScreenY / 2 - lHight / 2;
		SetWindowPos(pWnd, nScreenX, nScreenY, lRight, lHight, SWP_NOZORDER | SWP_NOSIZE);
		return;
	}

}

void CAirMsgBox::SetButton()
{
	CRect rc,rcDlg;
	this->GetWindowRect (&rcDlg);
	ScreenToClient (&rcDlg);
	BYTE bID = m_uID&0x00000000f;


	CSize size;
	HBITMAP hBitmapOK, hBitmapCancel;	
#if LOADBMP_FROM_INI

	hBitmapOK = CSkinMgr::Instance()->LoadBitmap(_T("Okex"), size);
	hBitmapCancel = CSkinMgr::Instance()->LoadBitmap(_T("Cancelex"), size);

#else

	m_BitmapOK.LoadBitmap(IDB_BITMAP_OKEX);
	hBitmapOK = (HBITMAP)m_BitmapOK.m_hObject;
	m_BitmapCancel.LoadBitmap(IDB_BITMAP_CANCELEX);
	hBitmapCancel = (HBITMAP)m_BitmapCancel.m_hObject;

#endif

	if(bID == MB_OKCANCEL || bID == MB_YESNO)
	{
		rc.left = rcDlg.Width () * 1 / 4 - BUTTONWIDTH / 2;
		rc.right = rc.left + BUTTONWIDTH;
		rc.bottom = rcDlg.bottom - 20;
		rc.top = rc.bottom - BUTTONHEIGHT;
		m_btnA.Create(_T("OK"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rc, this, IDOK);
		m_btnA.SetBitmap (hBitmapOK, RGB(255,0,255), FALSE);
		rc.left = rcDlg.Width () * 3 / 4 - BUTTONWIDTH / 2;
		rc.right = rc.left + BUTTONHEIGHT;
		m_btnB.Create(_T("CANCEL"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rc, this, IDCANCEL);
		m_btnB.SetBitmap (hBitmapCancel, RGB(255,0,255), FALSE);
	}
	else
	{
		rc.left = rcDlg.Width () / 2 - BUTTONWIDTH / 2;
		rc.right = rc.left + BUTTONWIDTH;
		rc.bottom = rcDlg.bottom - 20;
		rc.top = rc.bottom - BUTTONHEIGHT;
		m_btnA.Create(_T("OK"), WS_CHILD|WS_VISIBLE|WS_TABSTOP, rc, this, IDOK);
		m_btnA.SetBitmap (hBitmapOK, RGB(255,0,255), FALSE);
	}
}

void CAirMsgBox::SetPicture()
{
	LONG bID = m_uID&0x000000ff;

	CSize size;
	HBITMAP hBitmapStop, hBitmapQuestion, hBitmapWarnning;
#if LOADBMP_FROM_INI

	hBitmap = CSkinMgr::Instance()->LoadBitmap(_T("MsgStop"), size);
	hBitmap = CSkinMgr::Instance()->LoadBitmap(_T("MsgQuestion"), size);
	hBitmap = CSkinMgr::Instance()->LoadBitmap(_T("MsgWarnning"), size);

#else
	
	m_BitmapStop.LoadBitmap(IDB_BITMAP_STOP);
	hBitmapStop = (HBITMAP)m_BitmapStop.m_hObject;
	m_BitmapQuestion.LoadBitmap(IDB_BITMAP_QUESTION);
	hBitmapQuestion = (HBITMAP)m_BitmapQuestion.m_hObject;
	m_BitmapWarnning.LoadBitmap(IDB_BITMAP_WARNING);
	hBitmapWarnning = (HBITMAP)m_BitmapWarnning.m_hObject;

#endif

	HBITMAP hBitmap;
	if(bID == MB_ICONHAND)
	{
		hBitmap = hBitmapStop;
	}
	else if(bID == MB_ICONQUESTION)
	{
		hBitmap = hBitmapQuestion;
	}
	else
	{
		hBitmap = hBitmapWarnning;
	}
	m_pStaPic->ModifyStyle(NULL,SS_BITMAP,SWP_NOSIZE);
	m_pStaPic->SetBitmap(hBitmap);
}

void DoText(CDC* pDC, const CFont *pFont, const CString & strOld, CString & strNew, long lW = 410)
{
	SIZE se;
	CDC MemDC; //]
	MemDC.CreateCompatibleDC(pDC);
	MemDC.SelectObject((CFont *)pFont);
	GetTextExtentPoint32(MemDC.GetSafeHdc(), strOld, wcslen(strOld), &se); //获取字符串的宽度

	// Fixed width
	if(se.cx > lW)
	{
		int n = 10;
		CString strTemp = strOld.Left(n);
		BOOL bLast = FALSE;
		for (; n < strOld.GetLength(); n ++)
		{
			strTemp += strOld[n];
			GetTextExtentPoint32(MemDC.GetSafeHdc(), strTemp, wcslen(strTemp), &se);
			if(se.cx > lW)
			{
				strNew += strTemp + _T("\r\n");
				strTemp = strOld.Mid(n+1, 10);
				n += 10;

				if(n == strOld.GetLength()-1)
				{
					bLast = TRUE;
				}
			}
		}
		if(!bLast)
		{
			strNew += strTemp + _T("\r\n");
		}
	}
	else
	{
		strNew = strOld + _T("\r\n");
	}
}

void CAirMsgBox::FormatMsgContont()
{
	CDC* pDC = m_stcMsg.GetDC();
	m_strMsg.Replace(_T("\r\n"), _T("\r"));
	m_strMsg.Replace(_T('\n'), _T('\r'));
	int nFnd = m_strMsg.Find(_T('\r'));
	int nIdx = 0;
	CString strText;
	CString strT, strNew;
	while(nFnd != -1)
	{
		strText = m_strMsg.Mid(nIdx, nFnd-nIdx);
		strText.Trim(_T('\r'));
		if(!strText.IsEmpty())
		{
			DoText(pDC, m_pftYaheipx, strText, strT);
			strNew += strT;
		}

		nIdx = nFnd + 1;
		nFnd = m_strMsg.Find(_T('\r'), nIdx);
	}
	strText = m_strMsg.Mid(nIdx);
	if(!strText.IsEmpty())
	{
		strText.Trim(_T('\r'));
		DoText(pDC, m_pftYaheipx, strText, strT);
		strNew += strT;
	}

	strNew.TrimRight(_T("\r\n"));
	m_strMsg = strNew;

}