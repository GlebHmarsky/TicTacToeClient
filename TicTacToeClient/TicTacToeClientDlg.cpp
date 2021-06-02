
// TicTacToeClientDlg.cpp : implementation file
//

#include <Windows.h>
#include "pch.h"
#include "framework.h"
#include "TicTacToeClient.h"
#include "TicTacToeClientDlg.h"
#include "afxdialogex.h"
#include <sstream>
#include <iostream>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define DEFAULT_PORT	5150
#define DEFAULT_BUFFER	2048

UINT GetRecv(PVOID lpParam);

CPoint getPixelPoint(CPoint Lcenter);
POSITION* IsWinningStep();
void Refresh();

SOCKET	m_sClient;
HWND hWnd_LB;
HWND hWnd_LobbyL;
HWND hWnd_TB;
bool fIsCross = true;
bool canDoStep = false;
HDC m_hCDC;
double RX1 = 20, RY1 = 20, RX2 = 520, RY2 = 520; //������� ������� � ���������� �����������
CTicTacToeClientDlg* DlgExample;
CList<CPoint> lPoints;
// CTicTacToeClientDlg dialog
WSAEVENT hSingleEvent;

CTicTacToeClientDlg::CTicTacToeClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_TICTACTOECLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTicTacToeClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	DDX_Control(pDX, IDC_LOBBYLIST, m_LobbyList);
}

BEGIN_MESSAGE_MAP(CTicTacToeClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_ButtonRefresh, &CTicTacToeClientDlg::OnBnClickedButtonrefresh)
	ON_BN_CLICKED(IDC_CONNECT, &CTicTacToeClientDlg::OnBnClickedConnect)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DISCONNECT, &CTicTacToeClientDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_CREATELOBBY, &CTicTacToeClientDlg::OnBnClickedCreatelobby)
	ON_BN_CLICKED(IDC_LEAVELOBBY, &CTicTacToeClientDlg::OnBnClickedLeavelobby)
	ON_BN_CLICKED(IDC_CONNECTLOBBY, &CTicTacToeClientDlg::OnBnClickedConnectlobby)
	ON_LBN_SELCHANGE(IDC_LOBBYLIST, &CTicTacToeClientDlg::OnLbnSelchangeLobbylist)
	ON_BN_CLICKED(IDC_REMATCH, &CTicTacToeClientDlg::OnBnClickedRematch)
END_MESSAGE_MAP()


// CTicTacToeClientDlg message handlers
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTicTacToeClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CTicTacToeClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	setlocale(LC_ALL, "russian_russia.1251");
	setlocale(LC_CTYPE, "rus");
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	char Str[128];

	GetDlgItem(IDC_SERVER)->SetWindowText("localhost");
	sprintf_s(Str, sizeof(Str), "%d", DEFAULT_PORT);
	GetDlgItem(IDC_PORT)->SetWindowText(Str);

	hWnd_LB = m_ListBox.m_hWnd;   // ��� �������
	hWnd_LobbyL = m_LobbyList.m_hWnd;
	hWnd_TB = GetDlgItem(IDC_MESSAGE)->m_hWnd;
	GetDlgItem(IDC_MESSAGE)->SetWindowTextA("���������� �������������� � �������,\r\n����� �������");

	hSingleEvent = WSACreateEvent();

	DlgExample = this;
	CClientDC tmpCDC(this);
	m_hCDC = tmpCDC.m_hDC;

	accessButtons(1);

	this->SetWindowTextA("TicTacToe Game Client");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


/* --------------------		DRAWING	 ------------------*/

void CTicTacToeClientDlg::DrawLines()
{
	CPaintDC PaintDC(this);
	CPen m_LinePen;
	m_LinePen.CreatePen(PS_DEFAULT, 2, RGB(10, 10, 10));
	PaintDC.SelectObject(&m_LinePen);
	PaintDC.Rectangle(RX1, RY1, RX2, RY2);
	PaintDC.IntersectClipRect(RX1, RY1, RX2, RY2);

	/*--- ����� ----*/
	m_LinePen.DeleteObject();
	m_LinePen.CreatePen(PS_DEFAULT, 3, RGB(200, 200, 200));
	PaintDC.SelectObject(&m_LinePen);
	for (int pos = RY1 + (RY2 - RY1) / 3; pos < RY2; pos += (RY2 - RY1) / 3 + 1) {
		PaintDC.MoveTo(RX1, pos);
		PaintDC.LineTo(RX2, pos);
	}
	for (int pos = RX1 + (RX2 - RX1) / 3; pos < RX2; pos += (RX2 - RX1) / 3 + 1) {
		PaintDC.MoveTo(pos, RY1);
		PaintDC.LineTo(pos, RY2);
	}
}

void CTicTacToeClientDlg::DrawCross(int x, int y)
{
	CClientDC ClientDC(this);
	ClientDC.FromHandle(m_hCDC);
	//(CClientDC*)(CClientDC::FromHandle(m_hCDC))
	//ClientDC = (CClientDC*)(CClientDC::FromHandle(m_hCDC));
	ClientDC.IntersectClipRect(RX1, RY1, RX2, RY2);
	CPen pen;
	pen.CreatePen(PS_DEFAULT, 4, RGB(20, 20, 20));
	int distance = (RX2 - RX1) / 3; //������ ������� � ������� ����� ��������.
	int offset = (distance * 2) / 5; //������ �� ������ ��� �������
	ClientDC.SelectObject(&pen);

	ClientDC.MoveTo(x - offset, y - offset);
	ClientDC.LineTo(x + offset, y + offset);

	ClientDC.MoveTo(x - offset, y + offset);
	ClientDC.LineTo(x + offset, y - offset);


	//ReleaseDC(&ClientDC);
}

void CTicTacToeClientDlg::DrawCircle(int x, int y)
{
	CClientDC ClientDC(this);
	CPen pen;
	pen.CreatePen(PS_DEFAULT, 3, RGB(20, 20, 20));
	int distance = (RX2 - RX1) / 3; //������ ������� � ������� ����� ��������.
	int radius = distance * 2 / 5; //������ �� ������ ��� �������
	ClientDC.SelectObject(&pen);
	ClientDC.Ellipse(x - radius, y - radius, x + radius, y + radius);
	//ReleaseDC(&ClientDC);
}

void ReDrawAll() {
	DlgExample->DrawLines();
	CPoint Pcenter;
	int i = 0;
	POSITION pos;
	for (i = 0, pos = lPoints.GetHeadPosition(); pos != NULL; lPoints.GetNext(pos), i++) {
		Pcenter = getPixelPoint(lPoints.GetAt(pos));

		if (i % 2 == 0)
			DlgExample->DrawCross(Pcenter.x, Pcenter.y);
		else
			DlgExample->DrawCircle(Pcenter.x, Pcenter.y);
	}
}

void CTicTacToeClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		ReDrawAll();
	}
}


/* --------------------		WORK WITH TICTACTOE STUFF 	 ------------------*/

CPoint* FindTheSegment(CPoint point)
{
	if (point.x - RX1 <= 0 || point.y - RY1 <= 0)
		return NULL;

	int distance = (RX2 - RX1) / 3 + 3;
	int size = distance;
	int i, g;
	for (i = 0; i < 3; i++)
	{
		if (point.x - RX1 <= size)
			break;
		size += distance;
	}

	size = distance;
	for (g = 0; g < 3; g++)
	{
		if (point.y - RY1 <= size)
			break;
		size += distance;
	}
	if (i >= 3 || g >= 3)
	{
		return NULL;
	}
	return new CPoint(i, g);
}

void CTicTacToeClientDlg::MakeWinnerLine(POSITION* PosArr)
{
	CClientDC ClientDC(this);
	ClientDC.IntersectClipRect(RX1, RY1, RX2, RY2);
	CPen pen;
	pen.CreatePen(PS_DEFAULT, 7, RGB(20, 160, 20));
	ClientDC.SelectObject(&pen);

	int distance = (RX2 - RX1) / 3; //������ ������� � ������� ����� ��������.
	int offset = (distance * 2) / 9; //������ �� ������ ��� �������

	CPoint a = getPixelPoint(lPoints.GetAt(PosArr[0])),
		b = getPixelPoint(lPoints.GetAt(PosArr[1]));

	ClientDC.MoveTo(a.x, a.y);
	ClientDC.LineTo(b.x, b.y);
}

int GetIndexOfPosition(CList<CPoint>* list, POSITION pos)
{
	POSITION curPos = (*list).GetHeadPosition();
	CPoint elemnt = (*list).GetAt(pos);
	CPoint curEl;
	for (int i = 0; i < (*list).GetSize(); i++)
	{
		curEl = (*list).GetAt(curPos);
		if (elemnt == curEl)
		{
			return i;
		}
		if (curPos != NULL)
			(*list).GetNext(curPos);
		else
			break;
	}
	return -1;
}

CPoint getPixelPoint(CPoint Lcenter) {
	CPoint Pcenter(0, 0);
	int distance = (RX2 - RX1) / 3;

	Pcenter.x = distance / 2 + RX1;
	Pcenter.x += distance * Lcenter.x;

	Pcenter.y = distance / 2 + RY1;
	Pcenter.y += distance * Lcenter.y;

	return Pcenter;
}

POSITION* IsWinningStep()
{
	CPoint curPoint, tmpPoint, opTmpPoint;
	POSITION curPosInList = lPoints.GetTailPosition(),
		tmpPosition,
		opTmpPosition;


	curPoint = lPoints.GetAt(curPosInList);
	tmpPosition = lPoints.GetHeadPosition();
	if (!fIsCross && tmpPosition != NULL)
		lPoints.GetNext(tmpPosition);
	for (; tmpPosition != NULL; )
	{
		tmpPoint = lPoints.GetAt(tmpPosition);
		//���� �� ���� ���������,
		//�� ������ ���� ������� � �����-�� ������� � ���� ���� ��������� ��� ���� ������� � ���������������
		//��� ����� �� ������ �� ��������� ������� ����� ���� tmp ����� � ����:
		/*
		* �-�������: �� �� ��������� � cur �����
		* ��-������: ����� ����� ����
		* ��-������: � ������� � ������ ׸���� �������� (�������� ��������)
		*/
		if (tmpPoint == curPoint) {
			if (tmpPosition != NULL)
				lPoints.GetNext(tmpPosition);
			if (tmpPosition != NULL)
				lPoints.GetNext(tmpPosition);
			continue;
		}
		opTmpPoint.x = (curPoint.x * 2 - tmpPoint.x);
		opTmpPoint.y = (curPoint.y * 2 - tmpPoint.y);
		opTmpPosition = lPoints.Find(opTmpPoint);
		if (opTmpPosition != NULL)
		{
			int ret = GetIndexOfPosition(&lPoints, opTmpPosition);
			if (!(ret < 0))
			{
				if ((ret % 2 == 0 && fIsCross) || (ret % 2 != 0 && !fIsCross))//��������
				{
					if (abs(opTmpPoint.x - curPoint.x) == 2 || abs(opTmpPoint.y - curPoint.y) == 2)
					{
						return new POSITION[]{ curPosInList, opTmpPosition };
					}
					else if (abs(tmpPoint.x - curPoint.x) == 2 || abs(tmpPoint.y - curPoint.y) == 2)
					{
						return new POSITION[]{ curPosInList, tmpPosition };
					}
					else if (abs(tmpPoint.x - opTmpPoint.x) == 2 || abs(tmpPoint.y - opTmpPoint.y) == 2)
					{
						return new POSITION[]{ opTmpPosition, tmpPosition };
					}
				}
			}
		}
		opTmpPoint.x = ((tmpPoint.x * 2) - curPoint.x);
		opTmpPoint.y = ((tmpPoint.y * 2) - curPoint.y);
		opTmpPosition = lPoints.Find(opTmpPoint);
		if (opTmpPosition != NULL)
		{
			int ret = GetIndexOfPosition(&lPoints, opTmpPosition);
			if (!(ret < 0))
			{
				if ((ret % 2 == 0 && fIsCross) || (ret % 2 != 0 && !fIsCross))//��������
				{
					if (abs(opTmpPoint.x - curPoint.x) == 2 || abs(opTmpPoint.y - curPoint.y) == 2)
					{
						return new POSITION[]{ curPosInList, opTmpPosition };
					}
					else if (abs(tmpPoint.x - curPoint.x) == 2 || abs(tmpPoint.y - curPoint.y) == 2)
					{
						return new POSITION[]{ curPosInList, tmpPosition };
					}
					else if (abs(tmpPoint.x - opTmpPoint.x) == 2 || abs(tmpPoint.y - opTmpPoint.y) == 2)
					{
						return new POSITION[]{ opTmpPosition, tmpPosition };
					}
				}
			}
		}

		if (tmpPosition != NULL)
			lPoints.GetNext(tmpPosition);
		if (tmpPosition != NULL)
			lPoints.GetNext(tmpPosition);
	}
	return NULL;
}

void CTicTacToeClientDlg::accessButtons(int flag) {
	switch (flag)
	{
	case 0://Block all

		break;
	case 1: //While not connected
		GetDlgItem(IDC_SERVER)->EnableWindow(true);
		GetDlgItem(IDC_PORT)->EnableWindow(true);
		GetDlgItem(IDC_CONNECT)->EnableWindow(true);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(false);
		GetDlgItem(IDC_CONNECTLOBBY)->EnableWindow(false);
		GetDlgItem(IDC_CREATELOBBY)->EnableWindow(false);
		GetDlgItem(IDC_LEAVELOBBY)->EnableWindow(false);
		GetDlgItem(IDC_REMATCH)->EnableWindow(false);
		canDoStep = false;
		break;
	case 2://While connected && not in lobby
		GetDlgItem(IDC_SERVER)->EnableWindow(false);
		GetDlgItem(IDC_PORT)->EnableWindow(false);
		GetDlgItem(IDC_CONNECT)->EnableWindow(false);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDC_CONNECTLOBBY)->EnableWindow(true);
		GetDlgItem(IDC_CREATELOBBY)->EnableWindow(true);
		GetDlgItem(IDC_LEAVELOBBY)->EnableWindow(false);
		GetDlgItem(IDC_REMATCH)->EnableWindow(false);
		canDoStep = false;
		break;
	case 3://While connected && in lobby
		GetDlgItem(IDC_SERVER)->EnableWindow(false);
		GetDlgItem(IDC_PORT)->EnableWindow(false);
		GetDlgItem(IDC_CONNECT)->EnableWindow(false);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDC_CONNECTLOBBY)->EnableWindow(false);
		GetDlgItem(IDC_CREATELOBBY)->EnableWindow(false);
		GetDlgItem(IDC_LEAVELOBBY)->EnableWindow(true);
		GetDlgItem(IDC_REMATCH)->EnableWindow(false);
		break;

	case 4://While match is ended
		GetDlgItem(IDC_SERVER)->EnableWindow(false);
		GetDlgItem(IDC_PORT)->EnableWindow(false);
		GetDlgItem(IDC_CONNECT)->EnableWindow(false);
		GetDlgItem(IDC_DISCONNECT)->EnableWindow(true);
		GetDlgItem(IDC_CONNECTLOBBY)->EnableWindow(false);
		GetDlgItem(IDC_CREATELOBBY)->EnableWindow(false);
		GetDlgItem(IDC_LEAVELOBBY)->EnableWindow(true);
		GetDlgItem(IDC_REMATCH)->EnableWindow(true);
		break;
	default:
		break;
	}
}

/* -----------------------		BUTTONS & OTHER EVENTS	 ---------------------*/


/* --------------------		CONNECTION WITH SERVER	 ------------------*/

void CTicTacToeClientDlg::OnBnClickedConnect()
{
	// TODO: Add your control notification handler code here
	char		szServer[128];	// ��� ��� IP-����� �������
	int		iPort;			// ����

	WSADATA	wsd;

	struct sockaddr_in 	server;
	struct hostent* host = NULL; //��� ������� gethostbyname();

	char Str[256];

	CEdit* pTB = (CEdit*)(CEdit::FromHandle(hWnd_TB));

	GetDlgItem(IDC_SERVER)->GetWindowText(szServer, sizeof(szServer));
	GetDlgItem(IDC_PORT)->GetWindowText(Str, sizeof(Str));
	iPort = atoi(Str);
	if (iPort <= 0 || iPort >= 0x10000)
	{
		m_ListBox.AddString((LPTSTR)"Port number incorrect");
		return;
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		m_ListBox.AddString((LPTSTR)"Failed to load Winsock library!");
		pTB->SetWindowTextA("������ ��� �������� ���������� Winsock 2");
		return;
	}
	/*
	* ������� ������ ������� �����, ������� �������� � ������������� ���������� ������������ �����.
	*
	AF_INET(2) - ��������� ������� ��������-��������� ������ 4 (IPv4).
	*
	SOCK_STREAM(1) ��� ������, ������� ������������ ����������������, ��������, ������������ ��������
	* ������ �� ������ ���������� � ���������� �������� ������ OOB.
	* ���� ��� ������ ���������� �������� ���������� ��������� (TCP) ��� ��������� ��������-������� (AF_INET ��� AF_INET6).
	*
	IPPROTO_TCP(6)
	* �������� ���������� ��������� (TCP). ��� ��������� ��������, ���� �������� af ����� AF_INET ��� AF_INET6, � �������� ���� - SOCK_STREAM .
	*
	*/
	m_sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_sClient == INVALID_SOCKET)
	{
		sprintf_s(Str, sizeof(Str), "socket() failed: %d\n", WSAGetLastError());
		m_ListBox.AddString((LPTSTR)Str);
		pTB->SetWindowTextA("�� ������� ������� �����");
		return;
	}


	server.sin_family = AF_INET;
	/*
	* ������� htons ����������� u_short �� ����� � ������� ������� ������ TCP / IP (������� �������� ������ �������� ������).
	* ������� htons ���������� �������� � ������� ������� ������ TCP / IP.
	*/
	server.sin_port = htons(iPort);
	/*
	* ������� inet_addr ����������� ������, ���������� ���������� ����� IPv4 � �������, � ���������� ����� ��� ��������� IN_ADDR .
	* ���� ������ �� ���������, ������� inet_addr ���������� ������� �������� ��� �����,
	* ���������� ���������� �������� ������������� ������� ��������-������.
	*
	* ���� ������ � ��������� cp �� �������� ����������� ������ � ���������,
	* ��������, ���� ����� ������ �abcd� ��������� 255, �� inet_addr ���������� �������� INADDR_NONE .
	*
	*/
	server.sin_addr.s_addr = inet_addr(szServer);

	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		/*
		* ������� gethostbyname ��������� ���������� � �����, ��������������� ����� �����, �� ���� ������ �����.

		hostent * gethostbyname(
			const char *name
		);

		* ���� ������ �� ���������, gethostbyname ���������� ��������� �� ��������� �����, ��������� ����.
		* � ��������� ������ �� ���������� ������� ���������, � ���������� ����� ������ ����� ��������, ������ WSAGetLastError .
		*/
		host = gethostbyname(szServer);
		if (host == NULL)
		{
			sprintf_s(Str, sizeof(Str), "Unable to resolve server: %s", szServer);
			m_ListBox.AddString((LPTSTR)Str);
			pTB->SetWindowTextA("������ ������ �������");
			return;
		}
		CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
	}

	//������� connect ������������� ���������� � ��������� �������.
	if (connect(m_sClient, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "connect() failed: %d", WSAGetLastError());
		m_ListBox.AddString(Str);
		pTB->SetWindowTextA("�� ������� �������������� � �������");
		return;
	}
	pTB->SetWindowTextA("�� ������� �������������� � �������,\r\n������ ������� ����� ��� ������� ��� ������");

	accessButtons(2);
	AfxBeginThread(GetRecv, NULL);//��������� ��������� �� �������
}

void CTicTacToeClientDlg::OnBnClickedDisconnect()
{
	closesocket(m_sClient);

	WSACleanup();

	accessButtons(1);
	CEdit* pTB = (CEdit*)(CEdit::FromHandle(hWnd_TB));
	pTB->SetWindowTextA("�� ����������� �� �������,\r\n�� ������ ���������������� � ����� �����");
	CListBox* pLB = (CListBox*)(CListBox::FromHandle(hWnd_LB));
	pLB->ResetContent();
}

void CTicTacToeClientDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	OnBnClickedDisconnect();
	CDialog::OnClose();
}

/* --------------------		WORK WITH LOBBY	 ------------------*/

void CTicTacToeClientDlg::OnLbnSelchangeLobbylist()
{
	/*int nSel = m_LobbyList.GetCurSel();
	CString ItemSelected;
	m_LobbyList.GetText(nSel, ItemSelected);
	AfxMessageBox(ItemSelected);*/
}

void CTicTacToeClientDlg::OnBnClickedConnectlobby()
{
	int nSel;
	if ((nSel = m_LobbyList.GetCurSel()) < 0) {
		return;
	}
	CString ItemSelected;
	m_LobbyList.GetText(nSel, ItemSelected);
	char str[20];
	int nTokenPos = 0;
	CString strToken = ItemSelected.Tokenize(_T(" "), nTokenPos);
	strcpy(str, strToken.Mid(1));

	if (SendMessageToServer(str))
	{
		return;
	}
	accessButtons(3);
}

void CTicTacToeClientDlg::OnBnClickedCreatelobby()
{
	/*
	* ���� ������������ ���������� ���� ����� ��:
	* -1 - ��� ������� � ��������� �����
	* != 0 - ��� ������ ����� ������������ � ����������� ����� � ������ ������� �������� � ���� �� ������
	*/

	CString name;
	CEdit* pTB = (CEdit*)(CEdit::FromHandle(hWnd_TB));
	GetDlgItem(IDC_NICKNAME)->GetWindowText(name);
	char c;
	for (int i = 0; i < name.GetLength(); i++)
	{
		c = name.GetAt(i);
		if (c < 0) {
			MessageBox("������ ���, ������� ����� ����������");
			return;
		}
	}
	name = "-1 " + name;
	char* str = new char[name.GetLength() + 1];
	strcpy(str, name);

	if (SendMessageToServer(str))
	{
		return;
	}

	accessButtons(3);
	char* String("�� ������� ������� �����");
	pTB->SetWindowTextA(String);
}

void CTicTacToeClientDlg::OnBnClickedLeavelobby()
{
	// ���������� ��������� �� ������ ��� ����� ����� �� �����
	//����� ���� ����� �� ������� ������� ����� ����� ����������� � ��������� ��� ��������� �� ������ ����� � ��.
	CEdit* pTB = (CEdit*)(CEdit::FromHandle(hWnd_TB));
	char str[50];
	snprintf(str, 3, "-3");
	if (SendMessageToServer(str))
	{
		return;
	}
	accessButtons(2);
	pTB->SetWindowTextA("�� ������������� �� �����");
}

void CTicTacToeClientDlg::OnBnClickedRematch()
{
	//�������� ��������� �� ������ ������� ����� ������� �� ���, ����� ��� ������������ ���� �������� �� ��������� ����. 
	char str[50];
	snprintf(str, 3, "-2");
	if (SendMessageToServer(str))
	{
		return;
	}

}

void CTicTacToeClientDlg::OnBnClickedButtonrefresh()
{
	lPoints.RemoveAll();
	fIsCross = true;
	CRect GraficRect(RX1, RY1, RX2, RY2);
	InvalidateRect(GraficRect);
	UpdateWindow();
}

void Refresh() {
	DlgExample->OnBnClickedButtonrefresh();
}

void CTicTacToeClientDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!canDoStep)
		return;

	CPoint* Lcenter;
	if ((Lcenter = FindTheSegment(point)) == NULL) {
		return;
	}
	if (lPoints.Find(*Lcenter) != NULL)
	{
		return;  // � ��� ��� ���� ����� ������� � ���� ��� ��������� ��� ���
	}
	char str[128];
	snprintf(str, sizeof(str), "%ld %ld", Lcenter->x, Lcenter->y);
	if (SendMessageToServer(str))
	{
		return;
	}
	/*CPoint Pcenter = getPixelPoint(*Lcenter);

	if (fIsCross)
		DrawCross(Pcenter.x, Pcenter.y);
	else
		DrawCircle(Pcenter.x, Pcenter.y);

	lPoints.AddTail(*Lcenter);
	POSITION* PosArr;
	if ((PosArr = IsWinningStep()) != NULL) {
		MakeWinnerLine(PosArr);
		CString str = (fIsCross ? "��������" : "������");
		MessageBox("Win: " + str);
	}

	fIsCross = !fIsCross;*/
	CDialog::OnLButtonUp(nFlags, point);
}

/* --------------------		DIALOG WITH SERVER 	 ------------------*/

UINT CTicTacToeClientDlg::SendMessageToServer(const char* Buff)
{
	char	szMessage[1024];		// ��������� ��� ��������
	BOOL	bSendOnly = FALSE;	// ������ �������� ������

	char	szBuffer[DEFAULT_BUFFER];
	int		ret,
		i;

	char	Str[256];

	UpdateData();

	strcpy(szMessage, Buff);
	/*
	* ������� �������� ���������� ������ �� ������������ �����.
	*
	* ���� ������ �� ����������, send ���������� ����� ���������� ������������ ������,
	* ������� ����� ���� ������ ����������, ������������ ��� �������� � ��������� len.
	* � ��������� ������ ������������ �������� SOCKET_ERROR, � ���������� ��� ������ ����� ��������, ������ WSAGetLastError .
	*/
	ret = send(m_sClient, szMessage, strlen(szMessage), 0);

	if (ret == 0)
		return 0;
	else if (ret == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str), "send() failed: %d", WSAGetLastError());
		m_ListBox.AddString((LPTSTR)Str);
		return 1;
	}

	sprintf_s(Str, sizeof(Str), "Send %d bytes\n", ret);
	m_ListBox.AddString((LPTSTR)Str);

	return 0;
}

UINT GetRecv(PVOID lpParam) {
	DWORD Event;

	char	szBuffer[DEFAULT_BUFFER];
	int		ret, i;

	char	StrBuffer[256];
	char	Str[DEFAULT_BUFFER];
	CListBox* pLB = (CListBox*)(CListBox::FromHandle(hWnd_LB));
	CListBox* pLobbyL = (CListBox*)(CListBox::FromHandle(hWnd_LobbyL));
	CEdit* pTB = (CEdit*)(CEdit::FromHandle(hWnd_TB));
	WSANETWORKEVENTS NetworkEvents;
	DWORD Flags;
	DWORD RecvBytes;
	WSABUF DataBuffer;


	/*if (WSAEventSelect(m_sClient, hSingleEvent,
		FD_READ) == SOCKET_ERROR)
	{
		sprintf_s(Str, sizeof(Str),
			"WSAEventSelect() failed with error %d",
			WSAGetLastError());
		pLB->AddString((LPTSTR)Str);
		return;
	}*/

	while (TRUE)
	{
		/*	������� WSAWaitForMultipleEvents ������������, ����� ���� ��� ��� ��������� ������� ������� ��������� � ���������� ���������,
			����� �������� �������� ������� �������� ��� ����� ����������� ������������ ���������� �����-������.

			If the fWaitAll parameter is FALSE, the return value minus WSA_WAIT_EVENT_0
			(!!!) indicates the lphEvents array INDEX(!) of the signaled event object that satisfied the wait.
		*/
		/*if ((Event = WSAWaitForMultipleEvents(1,
			&hSingleEvent, FALSE, WSA_INFINITE, FALSE)) ==
			WSA_WAIT_FAILED)
		{
			sprintf_s(Str, sizeof(Str),
				"WSAWaitForMultipleEvents failed"
				" with error %d", WSAGetLastError());
			pLB->AddString(Str);
			return 1;
		}*/
		/*
		������� WSAEnumNetworkEvents ������������ ��������� ������� ������� ��� ���������� ������,
		������� ������ ���������� ������� ������� � ���������� ������� ������� (�������������).
		*/
		//if (WSAEnumNetworkEvents(
		//	m_sClient,
		//	&hSingleEvent,
		//	&NetworkEvents) == SOCKET_ERROR)
		//{
		//	sprintf_s(Str, sizeof(Str),
		//		"WSAEnumNetworkEvents failed"
		//		" with error %d", WSAGetLastError());
		//	pLB->AddString(Str);
		//	return 1;
		//}
		//if (NetworkEvents.lNetworkEvents & FD_READ)
		//{
		//	if (NetworkEvents.lNetworkEvents & FD_READ &&
		//		NetworkEvents.iErrorCode[FD_READ_BIT] != 0)
		//	{
		//		sprintf_s(Str, sizeof(Str),
		//			"FD_READ failed with error %d",
		//			NetworkEvents.iErrorCode[FD_READ_BIT]);
		//		pLB->AddString(Str);
		//		break;
		//	}
		//	DataBuffer.buf = szBuffer;
		//	DataBuffer.len = DEFAULT_BUFFER;
		//	//BUG ??? Flags - �� ������������ ����� ������
		//	Flags = 0;
		//	/*
		//	* ������� WSARecv �������� ������ �� ������������� ������ ��� ������������ ������ ��� ������������ ����������.
		//	*
		//	* ���� ������ �� ��������� � �������� ������ ����������� ����������, WSARecv ���������� ����.
		//	� ���� ������ ��������� ���������� ����� ��� ������������� ��� ������, ����� ���������� ����� �������� � ��������� ��������������.
		//	� ��������� ������ ������������ �������� SOCKET_ERROR , � ���������� ��� ������ ����� ��������, ������ WSAGetLastError
		//	*/
		//	
		//	if (WSARecv(
		//		m_sClient,
		//		&DataBuffer,
		//		1,
		//		&RecvBytes,
		//		&Flags, NULL, NULL) == SOCKET_ERROR)
		//	{
		//		if (WSAGetLastError() != WSAEWOULDBLOCK)
		//		{
		//			sprintf_s(Str, sizeof(Str),
		//				"WSARecv()failed with "
		//				" error %d", WSAGetLastError());
		//			pLB->AddString(Str);
		//			return 1;
		//		}
		//	}
		//	else
		//	{
		//		CString str(szBuffer, RecvBytes);


		/*
		* ������� recv �������� ������ �� ������������� ������ ��� ������������ ������ ��� ������������ ����������.
		* ���� ������ �� ����������, recv ���������� ���������� ���������� ������, �
		* �����, �� ������� ��������� �������� buf, ����� ��������� ��� ���������� ������.
		* ���� ���������� ���� ��������� �������, ������������ �������� ����� ����.
		*/
		ret = recv(m_sClient, szBuffer, DEFAULT_BUFFER, 0);
		if (ret == 0)	// ���������� ����������
			continue;
		else if (ret == SOCKET_ERROR)
		{
			sprintf_s(StrBuffer, sizeof(StrBuffer), "recv() failed: %d", WSAGetLastError());
			pLB->AddString((LPTSTR)StrBuffer);
			DlgExample->OnBnClickedDisconnect();//������� �� ������� �.�. ���������� no more
			return 1;
		}
		CString str(szBuffer, ret);


		if (szBuffer[0] != '#') //������ ���� ��� �� ��� �����
		{
			int nTokenPos = 0;
			int a = -1,
				b = -1;
			CString strToken = str.Tokenize(_T(" "), nTokenPos);
			while (!strToken.IsEmpty())
			{
				if (b >= 0)//������� �� �����
				{
					break;
				}
				if (a < 0)
					a = atoi(strToken);
				else
					b = atoi(strToken);
				strToken = str.Tokenize(_T("+"), nTokenPos);
			}

			if (b >= 0)//������� ������ �� �����
			{
				CPoint point(a, b);
				CPoint Pcenter = getPixelPoint(point);

				if (fIsCross)
					DlgExample->DrawCross(Pcenter.x, Pcenter.y);
				else
					DlgExample->DrawCircle(Pcenter.x, Pcenter.y);

				lPoints.AddTail(point);
				POSITION* PosArr;
				if ((PosArr = IsWinningStep()) != NULL) {
					CTicTacToeClientDlg dlg;
					DlgExample->MakeWinnerLine(PosArr);
					/*CString str = (fIsCross ? "��������" : "������");
					AfxMessageBox("�������� " + str);*/

					CString str = (canDoStep ? "�� ��������!" : "�� ���������...");
					pTB->SetWindowTextA(str);
					//AfxMessageBox(str);
					DlgExample->accessButtons(4);
					//���� ���������������.
					fIsCross = true;
					canDoStep = false;
					continue;
				}

				fIsCross = !fIsCross;
				canDoStep = !canDoStep;
			}
			else if (a == 0)
			{
				//��� ��������� �� �����
			}
			else if (a > 0)
			{
				switch (a)
				{
				case 1://�������� ������ �����
					pLobbyL->ResetContent();
					break;
				case 2://���� ��������� ������� ������
					DlgExample->accessButtons(3);
					canDoStep = false;
					pTB->SetWindowTextA("���� ��������� ������� ������");
					Refresh();
					break;
				case 3://������� ����� � ���� �� ����� ����������. + ���� ���� ��� �������� �� �� ����������
					DlgExample->accessButtons(3);
					canDoStep = false;
					pTB->SetWindowTextA("��� �������� ����� � �� �������� �� ����������� �������.\r\n��� �� �� ������ ��������� ���� ���-������\r\n ����� � ���");
					Refresh();
					break;

				case 4: //���� ����� �� ������
					DlgExample->accessButtons(1);
					pTB->SetWindowTextA("������ ����������\r\n���������� ����� ���� �����,\r\n ����� ����������� �����");
					Refresh();
					break;

				case 5: //���� �������� � ���� ������ ����� ������
					canDoStep = true;
					pTB->SetWindowTextA("���� ��������! �� ������ ������");
					DlgExample->accessButtons(3);
					break;
				case 6: //���� �������� � ���� ������ �� ����� ������
					canDoStep = false;
					pTB->SetWindowTextA("���� ��������! �� ������ ������");
					DlgExample->accessButtons(3);
					break;
				case 8: //�� ����� ������
					Refresh();
					//ADD �������� ��������� � ��� ��� �� ����� ������
					/*pTB->SetWindowTextA("������!");*/
					break;

				case 9: //�� ������� ������� �� ����� �� ������� ���������� �����������
					canDoStep = false;
					pTB->SetWindowTextA("�� ������� ������� �����\r\n�������� ���������� �����");
					DlgExample->accessButtons(2);
					break;
				default:
					break;
				}
			}
			pLB->AddString(str);
		}
		else
		{
			//������� ��������� � ������ �����
			pLobbyL->AddString(str);
		}
		/*	}
		}*/

	}//while
	return 0;
}

