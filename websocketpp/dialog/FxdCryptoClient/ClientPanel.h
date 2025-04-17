#pragma once


// CClientPanel
class CCryptoSocket;
class CClientPanel : public CWnd
{
	DECLARE_DYNAMIC(CClientPanel)

public:
	CClientPanel();
	virtual ~CClientPanel();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedChkBinance();
	afx_msg void OnBnClickedChkCoinbase();
	afx_msg void OnBnClickedChkByBit();
	afx_msg void OnBnClickedChkOkx();
	afx_msg void OnBnClickedChkBitget();
	afx_msg void OnBnClickedChkUsdt();
	afx_msg void OnBnClickedChkUsd();

protected:
	CButton* m_pBtnRun;	CButton* m_pBtnStop;
	CButton* m_pChkBinance;	CButton* m_pChkCoinbase; CButton* m_pChkBybit;
	CButton* m_pChkOkx;	CButton* m_pChkBitget;
	CButton* m_pChkBtc;	CButton* m_pChkEth;	CButton* m_pChkXrp; CButton* m_pChkEos;
	CButton* m_pChkTrx;	CButton* m_pChkUsdc; CButton* m_pChkBnb;
	CButton* m_pChkUsdt; CButton* m_pChkUsd;

public:
	CEdit* m_pEdtData; CEdit* m_pEdtLog;

	void AppendLog(LPCTSTR text, ...);

	void SetMarket();
	void SetSymbol();

	void EnableCheck();
	void DisableCheck();
	void ResetCheck();
	bool CheckCheck();

	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnStop();

	std::unique_ptr<CCryptoSocket> m_pCryptoSocket;

	// for thread
	std::queue<CString> m_queData;
	std::mutex m_mtxData;
	bool m_bRunning;
	void ReceivevData(std::string msg);
	void PostData();
	LRESULT PrintData(WPARAM wParam, LPARAM lParam);
};


