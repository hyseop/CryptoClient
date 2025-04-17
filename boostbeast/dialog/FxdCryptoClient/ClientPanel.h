#pragma once


// CClientPanel

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
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnStop();

protected:
	CButton* m_pBtnRun;	CButton* m_pBtnStop;
	CButton* m_pChkBinance;	CButton* m_pChkCoinbase; CButton* m_pChkBybit;
	CButton* m_pChkOkx;	CButton* m_pChkBitget;
	CButton* m_pChkBtc;	CButton* m_pChkEth;	CButton* m_pChkXrp; CButton* m_pChkEos;
	CButton* m_pChkTrx;	CButton* m_pChkUsdc; CButton* m_pChkBnb;
	CButton* m_pChkUsdt; CButton* m_pChkUsd;
	CEdit* m_pEdtData; CEdit* m_pEdtLog;

	CString m_strLog;

public:
	void AppendLog(LPCTSTR text, ...);
	void SetData(LPCTSTR text, ...);

	void SetUri();
	void SetSymbol();

	void EnableCheck();
	void DisableCheck();
	void ResetCheck();
	bool CheckCheck();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnStop();

//
//	//Crypto Socket
//private:
//	WSPPClient wsppClient;
//	websocketpp::connection_hdl m_hdl;
//	bool m_bInitialized;
//	bool m_bConnected;
//	std::string uri;
//	std::vector<std::string> symbols;
//
//	CWinThread* m_pThread = nullptr;
//
//protected:
//	void _Run();
//	void _Stop();
//	void _Init();
//	void _SetHandler();
//	void _Connect();
//	void _Subscribe(websocketpp::connection_hdl hdl);
//	void _ParseMessage(WSPPClient::message_ptr msg);
//	void SendMessage(websocketpp::connection_hdl hdl, const std::string& message);
};


