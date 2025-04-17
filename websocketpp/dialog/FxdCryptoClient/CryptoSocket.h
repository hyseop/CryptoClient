#pragma once


// CCryptoSocket
class CClientPanel;
class CCryptoSocket : public CWnd
{
	DECLARE_DYNAMIC(CCryptoSocket)

public:
	CCryptoSocket();
	virtual ~CCryptoSocket();

protected:
    DECLARE_MESSAGE_MAP()

private:
    WSPPClient wsppClient;

public:
    std::string m_market;
    std::vector<std::string> m_symbols;

    std::atomic<bool> stop_requested = false;
    std::thread read_thread;

public:
    void _SetMarket(const std::string& market);
    void _SetSymbol(const std::vector<std::string>& symbol);
    void _Run();
    void _threadRun();
    void _Stop();

private:
    void _Init();
    void _Connect();
    void _Subscribe(websocketpp::connection_hdl hdl);
    void _ReadMessage(WSPPClient::message_ptr msg);

	CClientPanel* m_pClientPanel;
public:
	void SetPanel(CClientPanel* pPanel) { m_pClientPanel = pPanel; }

};



