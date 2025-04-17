// ClientPanel.cpp: 구현 파일
//

#include "pch.h"
#include "FxdCryptoClient.h"
#include "ClientPanel.h"

#include "CryptoSocket.h"

#define WMU_PRINT_DATA (WM_USER + 1)

UINT threadPostData(LPVOID pParam) {
	CClientPanel* pThis = reinterpret_cast<CClientPanel*>(pParam);
	if (pThis) {
		while (pThis->m_bRunning) {
			pThis->PostData();
		}
	}
	return 0;
}

// CClientPanel

IMPLEMENT_DYNAMIC(CClientPanel, CWnd)

CClientPanel::CClientPanel()
	: m_pBtnRun(nullptr), m_pBtnStop(nullptr)

	, m_pChkBinance(nullptr), m_pChkCoinbase(nullptr), m_pChkBybit(nullptr)
	, m_pChkOkx(nullptr), m_pChkBitget(nullptr)

	, m_pChkBtc(nullptr), m_pChkEth(nullptr), m_pChkXrp(nullptr), m_pChkEos(nullptr)
	, m_pChkTrx(nullptr), m_pChkUsdc(nullptr), m_pChkBnb(nullptr)

	, m_pChkUsdt(nullptr), m_pChkUsd(nullptr)

	, m_pEdtData(nullptr), m_pEdtLog(nullptr)

	, m_pCryptoSocket(nullptr)

	, m_bRunning(false)
{

}

CClientPanel::~CClientPanel()
{
	PCH_DELETE(m_pBtnRun);
	PCH_DELETE(m_pBtnStop);
	PCH_DELETE(m_pChkBinance);
	PCH_DELETE(m_pChkCoinbase);
	PCH_DELETE(m_pChkBybit);
	PCH_DELETE(m_pChkOkx);
	PCH_DELETE(m_pChkBitget);
	PCH_DELETE(m_pChkBtc);
	PCH_DELETE(m_pChkEth);
	PCH_DELETE(m_pChkXrp);
	PCH_DELETE(m_pChkEos);
	PCH_DELETE(m_pChkTrx);
	PCH_DELETE(m_pChkUsdc);
	PCH_DELETE(m_pChkBnb);
	PCH_DELETE(m_pChkUsdt);
	PCH_DELETE(m_pChkUsd);
	PCH_DELETE(m_pEdtData);
	PCH_DELETE(m_pEdtLog);
}


BEGIN_MESSAGE_MAP(CClientPanel, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHK_BINANCE, &CClientPanel::OnBnClickedChkBinance)
	ON_BN_CLICKED(IDC_CHK_COINBASE, &CClientPanel::OnBnClickedChkCoinbase)
	ON_BN_CLICKED(IDC_CHK_BYBIT, &CClientPanel::OnBnClickedChkByBit)
	ON_BN_CLICKED(IDC_CHK_OKX, &CClientPanel::OnBnClickedChkOkx)
	ON_BN_CLICKED(IDC_CHK_BITGET, &CClientPanel::OnBnClickedChkBitget)
	ON_BN_CLICKED(IDC_CHK_USDT, &CClientPanel::OnBnClickedChkUsdt)
	ON_BN_CLICKED(IDC_CHK_USD, &CClientPanel::OnBnClickedChkUsd)
	ON_BN_CLICKED(IDC_BTN_RUN, &CClientPanel::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_STOP, &CClientPanel::OnBnClickedBtnStop)
	ON_MESSAGE(WMU_PRINT_DATA, &CClientPanel::PrintData)
END_MESSAGE_MAP()

// CClientPanel 메시지 처리기

int CClientPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pBtnRun = new CButton;
	PCH_CREATE_BTN(m_pBtnRun, "Run", IDC_BTN_RUN);

	m_pBtnStop = new CButton;
	PCH_CREATE_BTN(m_pBtnStop, "Stop", IDC_BTN_STOP);

	m_pChkBinance = new CButton;
	PCH_CREATE_CHK(m_pChkBinance, "Binance", IDC_CHK_BINANCE);

	m_pChkCoinbase = new CButton;
	PCH_CREATE_CHK(m_pChkCoinbase, "Coinbase", IDC_CHK_COINBASE);

	m_pChkBybit = new CButton;
	PCH_CREATE_CHK(m_pChkBybit, "Bybit", IDC_CHK_BYBIT);

	m_pChkOkx = new CButton;
	PCH_CREATE_CHK(m_pChkOkx, "Okx", IDC_CHK_OKX);

	m_pChkBitget = new CButton;
	PCH_CREATE_CHK(m_pChkBitget, "Bitget", IDC_CHK_BITGET);

	m_pChkBtc = new CButton;
	PCH_CREATE_CHK(m_pChkBtc, "BTC", IDC_CHK_BTC);

	m_pChkEth = new CButton;
	PCH_CREATE_CHK(m_pChkEth, "ETH", IDC_CHK_ETH);

	m_pChkXrp = new CButton;
	PCH_CREATE_CHK(m_pChkXrp, "XRP", IDC_CHK_XRP);

	m_pChkEos = new CButton;
	PCH_CREATE_CHK(m_pChkEos, "EOS", IDC_CHK_EOS);

	m_pChkTrx = new CButton;
	PCH_CREATE_CHK(m_pChkTrx, "TRX", IDC_CHK_TRX);

	m_pChkUsdc = new CButton;
	PCH_CREATE_CHK(m_pChkUsdc, "USDC", IDC_CHK_USDC);

	m_pChkBnb = new CButton;
	PCH_CREATE_CHK(m_pChkBnb, "BNB", IDC_CHK_BNB);

	m_pChkUsdt = new CButton;
	PCH_CREATE_CHK(m_pChkUsdt, "/USDT", IDC_CHK_USDT);

	m_pChkUsd = new CButton;
	PCH_CREATE_CHK(m_pChkUsd, "/USD", IDC_CHK_USD);

	m_pEdtData = new CEdit;
	PCH_CREATE_EDT(m_pEdtData, IDC_EDT_DATA);

	m_pEdtLog = new CEdit;
	PCH_CREATE_EDT(m_pEdtLog, IDC_EDT_LOG);

	if (!m_pCryptoSocket) {
		m_pCryptoSocket = std::make_unique<CCryptoSocket>();
		m_pCryptoSocket->SetPanel(this);
	}

	m_pChkBinance->SetCheck(BST_CHECKED);
	m_pChkBtc->SetCheck(BST_CHECKED);
	m_pChkUsdt->SetCheck(BST_CHECKED);

	return 0;
}

void CClientPanel::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	m_pChkBinance->MoveWindow(cx * 0 / 8, cy * 0 / 30, cx - (cx * 9 / 10), 25);
	m_pChkCoinbase->MoveWindow(cx * 1 / 8, cy * 0 / 30, cx - (cx * 9 / 10), 25);
	m_pChkBybit->MoveWindow(cx * 2 / 8, cy * 0 / 30, cx - (cx * 9 / 10), 25);
	m_pChkOkx->MoveWindow(cx * 3 / 8, cy * 0 / 30, cx - (cx * 9 / 10), 25);
	m_pChkBitget->MoveWindow(cx * 4 / 8, cy * 0 / 30, cx - (cx * 9 / 10), 25);

	m_pChkBtc->MoveWindow(cx * 0 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkEth->MoveWindow(cx * 1 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkXrp->MoveWindow(cx * 2 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkEos->MoveWindow(cx * 3 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkTrx->MoveWindow(cx * 4 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkUsdc->MoveWindow(cx * 5 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);
	m_pChkBnb->MoveWindow(cx * 6 / 8, cy * 1 / 30, cx - (cx * 9 / 10), 25);

	m_pChkUsdt->MoveWindow(cx * 0 / 8, cy * 2 / 30, cx - (cx * 9 / 10), 25);
	m_pChkUsd->MoveWindow(cx * 1 / 8, cy * 2 / 30, cx - (cx * 9 / 10), 25);

	m_pBtnRun->MoveWindow(cx * 6 / 8, cy * 3 / 30, cx - (cx * 9 / 10), 25);
	m_pBtnStop->MoveWindow(cx * 7 / 8, cy * 3 / 30, cx - (cx * 9 / 10), 25);

	m_pEdtData->MoveWindow(0, cy * 6 / 40, cx, cy - (cy * 11 / 30));
	m_pEdtLog->MoveWindow(0, cy * 25 / 30, cx, cy - (cy * 25 / 30));
}

HBRUSH CClientPanel::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	static CBrush brushWhite(RGB(255, 255, 255));
	static CBrush brushGray(RGB(245, 245, 245));
	static CBrush brushBlack(RGB(0, 0, 0));

	if (pWnd == m_pChkBinance || pWnd == m_pChkCoinbase || pWnd == m_pChkBybit
		|| pWnd == m_pChkOkx || pWnd == m_pChkBitget

		|| pWnd == m_pChkBtc || pWnd == m_pChkEth || pWnd == m_pChkXrp
		|| pWnd == m_pChkEos || pWnd == m_pChkTrx || pWnd == m_pChkUsdc
		|| pWnd == m_pChkBnb

		|| pWnd == m_pChkUsdt || pWnd == m_pChkUsd

		|| pWnd == m_pBtnRun || pWnd == m_pBtnStop) {
		pDC->SetBkColor(RGB(245, 245, 245));
		return brushGray;
	}

	if (pWnd == m_pEdtData || pWnd == m_pEdtLog) {
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(255, 255, 255));
		return brushBlack;
	}

	return hbr;
}

void CClientPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 그리기 메시지에 대해서는 CWnd::OnPaint()을(를) 호출하지 마십시오.

	CRect rect;
	GetClientRect(&rect);

	dc.FillSolidRect(&rect, RGB(245, 245, 245));
}

void CClientPanel::OnBnClickedChkBinance()
{
	if (m_pChkBinance->GetCheck() == BST_CHECKED) {
		ResetCheck();

		m_pChkCoinbase->SetCheck(BST_UNCHECKED);
		m_pChkBybit->SetCheck(BST_UNCHECKED);
		m_pChkOkx->SetCheck(BST_UNCHECKED);
		m_pChkBitget->SetCheck(BST_UNCHECKED);

		m_pChkTrx->EnableWindow(TRUE);
		m_pChkUsdc->EnableWindow(TRUE);
		m_pChkBnb->EnableWindow(TRUE);

		m_pChkUsd->EnableWindow(FALSE);
	}
}

void CClientPanel::OnBnClickedChkCoinbase()
{
	if (m_pChkCoinbase->GetCheck() == BST_CHECKED) {
		ResetCheck();

		m_pChkBinance->SetCheck(BST_UNCHECKED);
		m_pChkBybit->SetCheck(BST_UNCHECKED);
		m_pChkOkx->SetCheck(BST_UNCHECKED);
		m_pChkBitget->SetCheck(BST_UNCHECKED);

		m_pChkTrx->EnableWindow(FALSE);
		m_pChkUsdc->EnableWindow(FALSE);
		m_pChkBnb->EnableWindow(FALSE);

		m_pChkUsd->EnableWindow(TRUE);
	}
}

void CClientPanel::OnBnClickedChkByBit()
{
	if (m_pChkBybit->GetCheck() == BST_CHECKED) {
		ResetCheck();

		m_pChkBinance->SetCheck(BST_UNCHECKED);
		m_pChkCoinbase->SetCheck(BST_UNCHECKED);
		m_pChkOkx->SetCheck(BST_UNCHECKED);
		m_pChkBitget->SetCheck(BST_UNCHECKED);

		m_pChkTrx->EnableWindow(TRUE);
		m_pChkUsdc->EnableWindow(TRUE);
		m_pChkBnb->EnableWindow(FALSE);

		m_pChkUsd->EnableWindow(FALSE);
	}
}

void CClientPanel::OnBnClickedChkOkx()
{
	if (m_pChkOkx->GetCheck() == BST_CHECKED) {
		ResetCheck();

		m_pChkBinance->SetCheck(BST_UNCHECKED);
		m_pChkCoinbase->SetCheck(BST_UNCHECKED);
		m_pChkBybit->SetCheck(BST_UNCHECKED);
		m_pChkBitget->SetCheck(BST_UNCHECKED);

		m_pChkTrx->EnableWindow(TRUE);
		m_pChkUsdc->EnableWindow(TRUE);
		m_pChkBnb->EnableWindow(FALSE);

		m_pChkUsd->EnableWindow(FALSE);
	}
}

void CClientPanel::OnBnClickedChkBitget()
{
	if (m_pChkBitget->GetCheck() == BST_CHECKED) {
		ResetCheck();

		m_pChkBinance->SetCheck(BST_UNCHECKED);
		m_pChkCoinbase->SetCheck(BST_UNCHECKED);
		m_pChkBybit->SetCheck(BST_UNCHECKED);
		m_pChkOkx->SetCheck(BST_UNCHECKED);

		m_pChkTrx->EnableWindow(TRUE);
		m_pChkUsdc->EnableWindow(TRUE);
		m_pChkBnb->EnableWindow(FALSE);

		m_pChkUsd->EnableWindow(TRUE);
	}
}

void CClientPanel::OnBnClickedChkUsdt()
{
	if (m_pChkUsdt->GetCheck() == BST_CHECKED) {
		m_pChkUsd->SetCheck(BST_UNCHECKED);
	}
}

void CClientPanel::OnBnClickedChkUsd()
{
	if (m_pChkUsd->GetCheck() == BST_CHECKED) {
		m_pChkUsdt->SetCheck(BST_UNCHECKED);
	}
}

void CClientPanel::AppendLog(LPCTSTR text, ...)
{
	CString logAdded, msg;
	va_list va;
	SYSTEMTIME tm;

	GetLocalTime(&tm);
	va_start(va, text);
	msg.FormatV(text, va);
	va_end(va);

	logAdded.Format(_T("[%02d:%02d:%02d] %s"), tm.wHour, tm.wMinute, tm.wSecond, (LPCTSTR)msg);

	if (m_pEdtLog) {
		CString logExisted, logSetted;
		m_pEdtLog->GetWindowText(logExisted);

		if (!logExisted.IsEmpty()) {
			logSetted = logExisted += _T("\r\n");
		}
		logSetted += logAdded;

		m_pEdtLog->SetWindowText(logSetted);
		m_pEdtLog->LineScroll(m_pEdtLog->GetLineCount());
	}
	else {
		TRACE(logAdded);
	}
}

void CClientPanel::SetMarket()
{
	std::string market;

	if (m_pChkBinance->GetCheck()) {
		market = "wss://stream.binance.com:9443/ws/stream";
	}
	else if (m_pChkCoinbase->GetCheck()) {
		market = "wss://ws-feed.exchange.coinbase.com";
	}
	else if (m_pChkBybit->GetCheck()) {
		market = "wss://stream.bybit.com/v5/public/spot";
	}
	else if (m_pChkOkx->GetCheck()) {
		market = "wss://ws.okx.com:8443/ws/v5/public";
	}
	else if (m_pChkBitget->GetCheck()) {
		market = "wss://ws.bitget.com/spot/v1/stream";
	}

	if (m_pCryptoSocket) {
		m_pCryptoSocket->_SetMarket(market);
	}
}

void CClientPanel::SetSymbol()
{
	std::vector<std::string> symbols;
	symbols.clear();

	if (m_pChkBinance->GetCheck() == BST_CHECKED) {
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("btcusdt");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("ethusdt");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("xrpusdt");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("eosusdt");
		if (m_pChkTrx->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("trxusdt");
		if (m_pChkUsdc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("usdcusdt");
		if (m_pChkBnb->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("bnbusdt");
	}
	else if (m_pChkCoinbase->GetCheck() == BST_CHECKED) {
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("BTC-USDT");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("ETH-USDT");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("XRP-USDT");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("EOS-USDT");
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("BTC-USD");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("ETH-USD");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("XRP-USD");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("EOS-USD");
	}
	else if (m_pChkBybit->GetCheck() == BST_CHECKED) {
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("BTCUSDT");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("ETHUSDT");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("XRPUSDT");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("EOSUSDT");
		if (m_pChkTrx->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("TRXUSDT");
		if (m_pChkUsdc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("USDCUSDT");
	}
	else if (m_pChkOkx->GetCheck() == BST_CHECKED) {
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("BTC-USDT");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("ETH-USDT");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("XRP-USDT");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("EOS-USDT");
		if (m_pChkTrx->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("TRX-USDT");
		if (m_pChkUsdc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("USDC-USDT");
	}
	else if (m_pChkBitget->GetCheck() == BST_CHECKED) {
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("BTCUSDT");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("ETHUSDT");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("XRPUSDT");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("EOSUSDT");
		if (m_pChkTrx->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("TRXUSDT");
		if (m_pChkUsdc->GetCheck() == BST_CHECKED && m_pChkUsdt->GetCheck() == BST_CHECKED) symbols.push_back("USDCUSDT");
		if (m_pChkBtc->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("BTCUSD");
		if (m_pChkEth->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("ETHUSD");
		if (m_pChkXrp->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("XRPUSD");
		if (m_pChkEos->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("EOSUSD");
		if (m_pChkTrx->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("TRXUSD");
		if (m_pChkUsdc->GetCheck() == BST_CHECKED && m_pChkUsd->GetCheck() == BST_CHECKED) symbols.push_back("USDCUSD");
	}

	if (m_pCryptoSocket) {
		m_pCryptoSocket->_SetSymbol(symbols);
	}
}

void CClientPanel::EnableCheck()
{
	m_pChkBinance->EnableWindow(TRUE);
	m_pChkCoinbase->EnableWindow(TRUE);
	m_pChkBybit->EnableWindow(TRUE);
	m_pChkOkx->EnableWindow(TRUE);
	m_pChkBitget->EnableWindow(TRUE);
	m_pChkBtc->EnableWindow(TRUE);
	m_pChkEth->EnableWindow(TRUE);
	m_pChkXrp->EnableWindow(TRUE);
	m_pChkEos->EnableWindow(TRUE);
	m_pChkTrx->EnableWindow(TRUE);
	m_pChkUsdc->EnableWindow(TRUE);
	m_pChkBnb->EnableWindow(TRUE);
	m_pChkUsdt->EnableWindow(TRUE);
	m_pChkUsd->EnableWindow(TRUE);
}

void CClientPanel::DisableCheck()
{
	m_pChkBinance->EnableWindow(FALSE);
	m_pChkCoinbase->EnableWindow(FALSE);
	m_pChkBybit->EnableWindow(FALSE);
	m_pChkOkx->EnableWindow(FALSE);
	m_pChkBitget->EnableWindow(FALSE);
	m_pChkBtc->EnableWindow(FALSE);
	m_pChkEth->EnableWindow(FALSE);
	m_pChkXrp->EnableWindow(FALSE);
	m_pChkEos->EnableWindow(FALSE);
	m_pChkTrx->EnableWindow(FALSE);
	m_pChkUsdc->EnableWindow(FALSE);
	m_pChkBnb->EnableWindow(FALSE);
	m_pChkUsdt->EnableWindow(FALSE);
	m_pChkUsd->EnableWindow(FALSE);
}

void CClientPanel::ResetCheck()
{
	m_pChkBtc->SetCheck(BST_UNCHECKED);
	m_pChkEth->SetCheck(BST_UNCHECKED);
	m_pChkXrp->SetCheck(BST_UNCHECKED);
	m_pChkEos->SetCheck(BST_UNCHECKED);
	m_pChkTrx->SetCheck(BST_UNCHECKED);
	m_pChkUsdc->SetCheck(BST_UNCHECKED);
	m_pChkBnb->SetCheck(BST_UNCHECKED);

	m_pChkUsdt->SetCheck(BST_UNCHECKED);
	m_pChkUsd->SetCheck(BST_UNCHECKED);
}

bool CClientPanel::CheckCheck()
{
	if (m_pChkBinance->GetCheck() == BST_UNCHECKED
		&& m_pChkCoinbase->GetCheck() == BST_UNCHECKED
		&& m_pChkBybit->GetCheck() == BST_UNCHECKED
		&& m_pChkOkx->GetCheck() == BST_UNCHECKED
		&& m_pChkBitget->GetCheck() == BST_UNCHECKED) {
		AppendLog(_T("Check market at least one."));
		return false;
	}
	else if (m_pChkBtc->GetCheck() == BST_UNCHECKED
		&& m_pChkEth->GetCheck() == BST_UNCHECKED
		&& m_pChkXrp->GetCheck() == BST_UNCHECKED
		&& m_pChkEos->GetCheck() == BST_UNCHECKED
		&& m_pChkTrx->GetCheck() == BST_UNCHECKED
		&& m_pChkUsdc->GetCheck() == BST_UNCHECKED
		&& m_pChkBnb->GetCheck() == BST_UNCHECKED) {
		AppendLog(_T("Check base currency at least one."));
		return false;
	}
	else if (m_pChkUsdt->GetCheck() == BST_UNCHECKED
		&& m_pChkUsd->GetCheck() == BST_UNCHECKED) {
		AppendLog(_T("Check quote currency at least one."));
		return false;
	}
	return true;
}

void CClientPanel::OnBnClickedBtnRun()
{
	if (!CheckCheck()) {
		return;
	} 

	AppendLog(_T("Running..."));
	
	SetMarket();
	SetSymbol();

	if (m_pCryptoSocket) {
		m_pCryptoSocket->_threadRun();
	}
	if (!m_bRunning) {
		m_bRunning = true;
		AfxBeginThread(threadPostData, this);
	}
}

void CClientPanel::OnBnClickedBtnStop()
{
	AppendLog(_T("Stopping..."));

	if (m_pCryptoSocket) {
		m_pCryptoSocket->_Stop();
		m_pCryptoSocket.reset();
	}
	m_bRunning = false;
}

void CClientPanel::ReceivevData(std::string msg)
{
	std::lock_guard<std::mutex> lock(m_mtxData);
	m_queData.push(CString(msg.c_str()));
}

void CClientPanel::PostData() {
	CString data;
	{
		std::lock_guard<std::mutex> lock(m_mtxData);
		if (!m_queData.empty()) {
			data = m_queData.front();
			m_queData.pop();
		}
	}

	if (!data.IsEmpty()) {
		PostMessage(WMU_PRINT_DATA, 0, (LPARAM)new CString(data));
	}
}

LRESULT CClientPanel::PrintData(WPARAM wParam, LPARAM lParam)
{
	CString* pData = reinterpret_cast<CString*>(lParam);

	if (pData) {
		CString dataAdded = *pData;
		CString dataExisted, dataSetted;

		m_pEdtData->GetWindowText(dataExisted);

		if (!dataExisted.IsEmpty()) {
			dataSetted = dataExisted += _T("\r\n");
		}
		dataSetted += dataAdded;

		m_pEdtData->SetWindowText(dataSetted);
		m_pEdtData->LineScroll(m_pEdtData->GetLineCount());

		delete pData;
	}
	return 0;
}