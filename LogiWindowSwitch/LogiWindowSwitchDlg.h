
// LogiWindowSwitchDlg.h : 標頭檔
//

#pragma once
#include <vector>

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

	struct Group {
		std::vector<CWnd*> windows;
		int counter = 0;
		ULONGLONG timer = 0;
	};
	std::map<int, Group> groups;

	bool ctrl_is_down;
	bool shift_is_down;
	int foreground_group;
	std::vector<CWnd*> foreground_group_windows;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	BOOL SetRawInput(HWND hWnd);
	VOID RawInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	VOID HandleKeyInput(RAWKEYBOARD rawKB);
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	VOID PrintMessage(CString msg);
	BOOL IsAccepedWindow(CWnd* window);
public:
	CEdit m_Edit1;
	CListBox m_KeyList;
	CEdit m_WindowsEdit;
};
