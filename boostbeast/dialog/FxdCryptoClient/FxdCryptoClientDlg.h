﻿
// FxdCryptoClientDlg.h: 헤더 파일
//

#pragma once


// CFxdCryptoClientDlg 대화 상자
class CClientPanel;
class CFxdCryptoClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CFxdCryptoClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CFxdCryptoClientDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FXDCRYPTOCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

protected:
	CClientPanel* m_pClientPanel;
};
