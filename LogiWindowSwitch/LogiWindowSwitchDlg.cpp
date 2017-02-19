
// LogiWindowSwitchDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "LogiWindowSwitch.h"
#include "LogiWindowSwitchDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CALLBACKMSG (WM_USER + 1)

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
	DDX_Control(pDX, IDC_KEYLIST, m_KeyList);
	DDX_Control(pDX, IDC_WINDOWSLIST, m_WindowsList);
}

BEGIN_MESSAGE_MAP(CLogiWindowSwitchDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
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
	ctrl_is_down = false;
	shift_is_down = false;
	last_window = NULL;
	for (int i = 0; i < 9; i++) {
		CString kn;
		kn.Format(L"G%d", i + 1);
		m_KeyList.AddString(kn);
	}
	m_KeyList.SetCurSel(0);
	SetTimer(0, 100, NULL);
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
	bool down = flags % 2 == 0;
	
	if (scanCode == 29) {
		ctrl_is_down = down;
	}
	if (scanCode == 42 || scanCode == 54) {
		shift_is_down = down;
	}
	
	if (scanCode < 100 || scanCode >= 112) {
		return;
	}

	CString msg;
	auto &group = groups[scanCode];
	auto &windows = group.windows;
	if (ctrl_is_down || shift_is_down) {
		if (down) {
			if (ctrl_is_down) {
				windows.clear();
			}
			auto w = GetForegroundWindow();
			if (std::find(windows.begin(), windows.end(), w) == windows.end()) {
				windows.push_back(w);
			}
			CString tmp;
			w->GetWindowTextW(tmp);
			msg.Format(L"Set G%d : %s\r\n", scanCode - 99, tmp);
			PrintMessage(msg);
		}
	}
	else {
		if (down) {
			if (group.counter == 0) {
				for (auto w : windows) {
					if (w->IsIconic()) {
						auto fg = GetForegroundWindow();
						w->ShowWindow(SW_RESTORE);
						while (GetForegroundWindow() != fg) {
							fg->SetForegroundWindow();
						}
					}
					w->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}
				group.counter = 1;
				msg.Format(L"G%d down\r\n", scanCode - 99);
				PrintMessage(msg);
			}
			else if (group.counter == 1) {
				group.counter = 2;
				msg.Format(L"G%d long\r\n", scanCode - 99);
				PrintMessage(msg);
			}
		}
		else {
			for (auto w : windows) {
				w->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				if (group.counter == 1) {
					while (GetForegroundWindow() != w) {
						w->SetForegroundWindow();
					}
				}
				else if (group.counter == 2) {
					auto fg = GetForegroundWindow();
					fg->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				}
			}
			group.counter = 0;
			msg.Format(L"G%d Up\r\n", scanCode - 99);
			PrintMessage(msg);
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

void CLogiWindowSwitchDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 0) {
		for (auto &gp : groups) {
			auto &windows = gp.second.windows;
			for (int i = 0; i < windows.size(); i++) {
				if (!IsWindow(windows[i]->m_hWnd)) {
					windows.erase(windows.begin() + i);
					i--;
				}
			}
		}
		m_WindowsList.ResetContent();
		int key = m_KeyList.GetCurSel() + 100;
		for (int i = 0; i < groups[key].windows.size(); i++) {
			CString title;
			groups[key].windows[i]->GetWindowTextW(title);
			m_WindowsList.AddString(title);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

VOID CLogiWindowSwitchDlg::PrintMessage(CString msg) {
	CString str;
	m_Edit1.GetWindowTextW(str);
	str.Append(msg);
	m_Edit1.SetWindowTextW(str);
	m_Edit1.SetSel(0xFFFFFFF, 0xFFFFFFF);
}