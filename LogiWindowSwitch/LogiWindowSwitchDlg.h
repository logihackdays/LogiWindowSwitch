
// LogiWindowSwitchDlg.h : ���Y��
//

#pragma once


// CLogiWindowSwitchDlg ��ܤ��
class CLogiWindowSwitchDlg : public CDialogEx
{
// �غc
public:
	CLogiWindowSwitchDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIWINDOWSWITCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
protected:
	HICON m_hIcon;

	struct Window {
		CWnd *target;
		int counter;
	};
	std::map<int, Window> windows;

	bool ctrl_is_down;
	CWnd *last_window;

	// ���ͪ��T�������禡
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
public:
	CEdit m_Edit1;
};
