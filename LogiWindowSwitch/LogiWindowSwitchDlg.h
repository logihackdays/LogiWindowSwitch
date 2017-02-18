
// LogiWindowSwitchDlg.h : 標頭檔
//

#pragma once


// CLogiWindowSwitchDlg 對話方塊
class CLogiWindowSwitchDlg : public CDialogEx
{
// 建構
public:
	CLogiWindowSwitchDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIWINDOWSWITCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
