#pragma once


// CCryptoSocket

class CCryptoSocket : public CWnd
{
	DECLARE_DYNAMIC(CCryptoSocket)

public:
	CCryptoSocket();
	virtual ~CCryptoSocket();

protected:
	DECLARE_MESSAGE_MAP()

private:
    boost::asio::io_context ioc;
    boost::asio::ssl::context ssl_ctx;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>> ws;

public:
    std::string host;
    std::string port;
    std::string uri;
    std::vector<std::string> symbols;

    std::function<void(const std::string&)> onReadCallback;
    std::atomic<bool> stop_requested = false;
    std::thread read_thread;

public:
    void _SetCallback(const std::function<void(const std::string&)>& callback);
    void _SetMarket(const std::string& market);
    void _SetSymbols(const std::string& input);
    void _Run();
    void _Stop();

private:
    void _Connect();
    void _Subscribe();
    void _ReadMessages();
};


