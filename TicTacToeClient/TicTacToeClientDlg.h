
// TicTacToeClientDlg.h : header file
//

#pragma once
#include <iostream>
#include <list>

// CTicTacToeClientDlg dialog
class CTicTacToeClientDlg : public CDialog
{
	// Construction
public:
	CTicTacToeClientDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TICTACTOECLIENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	//double A = 0, B = 0, C = 0, D = 0; //Границы рисунка в логических коорданатах
	


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	CListBox m_ListBox;
	
	
	void accessButtons(int flag);

	void DrawLines();
	void DrawCross(int x, int y);
	void DrawCircle(int x, int y);


	//CPoint* FindTheSegment(CPoint point);
	//CPoint getPixelPoint(CPoint Lcenter);
	//POSITION* IsWinningStep();
	void MakeWinnerLine(POSITION* PosArr);
	UINT SendMessageToServer(const char* Buff);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonrefresh();
	afx_msg void OnBnClickedConnect();

	afx_msg void OnClose();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedCreatelobby();
	afx_msg void OnBnClickedLeavelobby();
	afx_msg void OnBnClickedConnectlobby();
	afx_msg void OnLbnSelchangeLobbylist();
	CListBox m_LobbyList;
	afx_msg void OnBnClickedRematch();
};
