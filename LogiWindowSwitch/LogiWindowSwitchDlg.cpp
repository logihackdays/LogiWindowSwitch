
// LogiWindowSwitchDlg.cpp : ��@��
//

#include "stdafx.h"
#include "LogiWindowSwitch.h"
#include "LogiWindowSwitchDlg.h"
#include "afxdialogex.h"
#include "LogitechGkeyLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CALLBACKMSG (WM_USER + 1)

// CLogiWindowSwitchDlg ��ܤ��



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
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLogiWindowSwitchDlg �T���B�z�`��

BOOL CLogiWindowSwitchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �]�w����ܤ�����ϥܡC�����ε{�����D�������O��ܤ���ɡA
	// �ج[�|�۰ʱq�Ʀ��@�~
	SetIcon(m_hIcon, TRUE);			// �]�w�j�ϥ�
	SetIcon(m_hIcon, FALSE);		// �]�w�p�ϥ�

	// TODO: �b���[�J�B�~����l�]�w
	ctrl_is_down = false;
	last_window = NULL;
	SetTimer(0, 100, NULL);
	if (!SetRawInput(m_hWnd)) {
		AfxMessageBox(L"Fail to Rigister Raw Input");
		exit(0);
	}
	return TRUE;  // �Ǧ^ TRUE�A���D�z�ﱱ��]�w�J�I
}

// �p�G�N�̤p�ƫ��s�[�J�z����ܤ���A�z�ݭn�U�C���{���X�A
// �H�Kø�s�ϥܡC���ϥΤ��/�˵��Ҧ��� MFC ���ε{���A
// �ج[�|�۰ʧ������@�~�C

void CLogiWindowSwitchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ø�s���˸m���e

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N�ϥܸm����Τ�ݯx��
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �yø�ϥ�
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ��ϥΪ̩즲�̤p�Ƶ����ɡA
// �t�ΩI�s�o�ӥ\����o�����ܡC
HCURSOR CLogiWindowSwitchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CLogiWindowSwitchDlg::HandleKeyInput(RAWKEYBOARD rawKB) {
	auto vkCode = rawKB.VKey;
	auto scanCode = rawKB.MakeCode;
	auto flags = rawKB.Flags;
	auto message = rawKB.Message;
	if (scanCode == 29) {
		ctrl_is_down = flags % 2 == 0;
	}
	if (scanCode < 100 || scanCode >= 112) {
		return;
	}

	CString msg;
	auto &window = windows[scanCode];
	if (ctrl_is_down) {
		if (flags % 2 == 0) {
			window.target = GetForegroundWindow();
			CString tmp;
			window.target->GetWindowTextW(tmp);
			msg.Format(L"Set G%d : %s\r\n", scanCode - 99, tmp);
			PrintMessage(msg);
		}
	}
	else {
		if (window.target == NULL || !IsWindow(window.target->m_hWnd)) {
			window.target = NULL;
		}
		else if (flags % 2 == 0) {
			if (window.counter == 0) {
				if (GetForegroundWindow() == window.target) {
					//window.target->SetWindowPos(&wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOREDRAW | SWP_NOSENDCHANGING);
					window.target->ShowWindow(SW_MINIMIZE);
				}
				else {
					if (window.target->IsIconic()) {
						auto fg = GetForegroundWindow();
						window.target->ShowWindow(SW_RESTORE);
						while (GetForegroundWindow() != fg) {
							fg->SetForegroundWindow();
						}
					}
					window.target->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
					window.counter = 1;
				}
				msg.Format(L"G%d down\r\n", scanCode - 99);
				PrintMessage(msg);
			}
			else if (window.counter == 1) {
				window.counter = 2;
				msg.Format(L"G%d long\r\n", scanCode - 99);
				PrintMessage(msg);
			}
		}
		else {
			window.target->SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			if (window.counter == 1) {
				while (GetForegroundWindow() != window.target) {
					window.target->SetForegroundWindow();
				}
			}
			else if (window.counter == 2) {
				auto fg = GetForegroundWindow();
				fg->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			window.counter = 0;
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
		for (auto window : windows) {
			if (window.second.target && !IsWindow(window.second.target->m_hWnd)) {
				window.second.target = NULL;
			}
		}
	}
	CString status;
	for (int i = 0; i < 9; i++) {
		CWnd *target = windows[i + 100].target;
		CString title;
		if (target && IsWindow(target->m_hWnd)) {
			target->GetWindowText(title);
		}
		status.AppendFormat(L"G%d : %s\n", i + 1, title);
	}
	GetDlgItem(IDC_STATIC)->SetWindowTextW(status);
	CDialogEx::OnTimer(nIDEvent);
}

VOID CLogiWindowSwitchDlg::PrintMessage(CString msg) {
	CString str;
	m_Edit1.GetWindowTextW(str);
	str.Append(msg);
	m_Edit1.SetWindowTextW(str);
	m_Edit1.SetSel(0xFFFFFFF, 0xFFFFFFF);
}