
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