
// ChromeTesterDlg.h: 头文件
//

#pragma once


// CChromeTesterDlg 对话框
class CChromeTesterDlg : public CDialogEx
{
// 构造
public:
	CChromeTesterDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CChromeTesterDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHROMETESTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();

protected:
	CString				m_strInputUrl;
	CString				m_strInputTemplateXml;
	CString				m_strInputMosId;
	CString				m_strInputRoomName;
	BOOL				m_bLocalList;
	CWnd*				m_pChromeWnd;
};
