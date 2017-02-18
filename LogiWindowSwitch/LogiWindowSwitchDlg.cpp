
// LogiWindowSwitchDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "LogiWindowSwitch.h"
#include "LogiWindowSwitchDlg.h"
#include "afxdialogex.h"
#include "LogitechGkeyLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogiWindowSwitchDlg 對話方塊



CLogiWindowSwitchDlg::CLogiWindowSwitchDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LOGIWINDOWSWITCH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogiWindowSwitchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit1);
}

BEGIN_MESSAGE_MAP(CLogiWindowSwitchDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CLogiWindowSwitchDlg 訊息處理常式

BOOL CLogiWindowSwitchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	if (!SetRawInput(m_hWnd)) {
		AfxMessageBox(L"Fail to Rigister Raw Input");
		exit(0);
	}
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CLogiWindowSwitchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CLogiWindowSwitchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CLogiWindowSwitchDlg::HandleKeyInput(RAWKEYBOARD rawKB) {
	auto vkCode = rawKB.VKey;
	auto scanCode = rawKB.MakeCode;
	auto flags = rawKB.Flags;
	auto message = rawKB.Message;
	CString ss;
	m_Edit1.GetWindowTextW(ss);
	ss.Format(L"%sdown vkcode : %d, scancode : %d, flags : %d\r\n", ss, vkCode, scanCode, flags);
	m_Edit1.SetWindowTextW(ss);
	m_Edit1.SetSel(0xFFFF, 0xFFFF);
	static CWnd *target[4];
	static clock_t timer[4];
	static bool start[4];

	int id = scanCode - 100;
	if (id < 0 || id >= 4)
		return;

	if (flags == 0) {
		if (start[id] == false) {
			start[id] = true;
			timer[id] = clock();
		}
		return;
	}
	start[id] = false;
	bool mode = (float(clock() - timer[id]) / CLOCKS_PER_SEC) >= 2;
	if (mode) {
		target[id] = GetForegroundWindow();
	}
	else {
		if (target[id] == NULL || !IsWindow(target[id]->m_hWnd)) {
			target[id] = NULL;
		}
		else {
			if (target[id]->IsIconic()) {
				target[id]->ShowWindow(SW_RESTORE);
			}
			while (GetForegroundWindow() != target[id]) {
				target[id]->SetActiveWindow();
				target[id]->SetForegroundWindow();
				target[id]->SetFocus();
			}
		}
	}
}

BOOL CLogiWindowSwitchDlg::SetRawInput(HWND hWnd) {
	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x06;
	Rid.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
	Rid.hwndTarget = hWnd;
	return RegisterRawInputDevices(&Rid, 1, sizeof(Rid));
}

VOID CLogiWindowSwitchDlg::RawInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	UINT dwSize;

	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb == NULL) {
		return;
	}

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
		sizeof(RAWINPUTHEADER)) != dwSize)
		OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

	RAWINPUT* raw = (RAWINPUT*)lpb;
	HandleKeyInput(raw->data.keyboard);
	delete[] lpb;
}

BOOL CLogiWindowSwitchDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_INPUT)
		RawInput(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
	return CDialog::PreTranslateMessage(pMsg);
}