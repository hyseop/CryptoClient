// CryptoSocket.cpp: 구현 파일
//

#include "pch.h"
#include "FxdCryptoClient.h"
#include "CryptoSocket.h"

#include "ClientPanel.h"

UINT threadRun(LPVOID pParam) {
    CCryptoSocket* pThis = reinterpret_cast<CCryptoSocket*>(pParam);
    if (pThis) {
        pThis->_Run();
    }
    return 0;
}

// CCryptoSocket

IMPLEMENT_DYNAMIC(CCryptoSocket, CWnd)

CCryptoSocket::CCryptoSocket()
	: m_pClientPanel(nullptr)
{

}

CCryptoSocket::~CCryptoSocket()
{
	_Stop();
}

BEGIN_MESSAGE_MAP(CCryptoSocket, CWnd)
END_MESSAGE_MAP()


// CCryptoSocket 메시지 처리기

void CCryptoSocket::_SetMarket(const std::string& market)
{
	m_market = market;
}

void CCryptoSocket::_SetSymbol(const std::vector<std::string>& symbols)
{
    m_symbols = symbols;
}

void CCryptoSocket::_Run()
{
    _Init();
    _Connect();
    wsppClient.run();
}

void CCryptoSocket::_threadRun()
{
	AfxBeginThread(threadRun, this);
}

void CCryptoSocket::_Stop()
{
    wsppClient.stop();
    //m_pClientPanel.reset();
}

void CCryptoSocket::_Init()
{
    wsppClient.init_asio();
    wsppClient.set_tls_init_handler([](websocketpp::connection_hdl) {
        return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
        });

    wsppClient.set_open_handler([this](websocketpp::connection_hdl hdl) {
        _Subscribe(hdl);
        });

    wsppClient.set_message_handler([this](websocketpp::connection_hdl hdl, WSPPClient::message_ptr msg) {
        _ReadMessage(msg);
        });

    wsppClient.clear_access_channels(websocketpp::log::alevel::all);
    wsppClient.clear_error_channels(websocketpp::log::elevel::all);
}

void CCryptoSocket::_Connect()
{
    websocketpp::lib::error_code ec;
    auto connection = wsppClient.get_connection(m_market, ec);
    if (ec) {
        throw std::runtime_error("Connection failed: " + ec.message());
    }
    wsppClient.connect(connection);
}

void CCryptoSocket::_Subscribe(websocketpp::connection_hdl hdl)
{
    try {
        json subscriptionMessage;

        if (m_market.find("binance") != std::string::npos) {
            subscriptionMessage = {
                {"method", "SUBSCRIBE"},
                {"params", json::array()},
                {"id", 1}
            };
            for (const auto& symbol : m_symbols) {
                subscriptionMessage["params"].push_back(symbol + "@depth");
            }
        }
        else if (m_market.find("coinbase") != std::string::npos) {
            subscriptionMessage = {
                {"type", "subscribe"},
                {"channels", json::array()}
            };
            for (const auto& symbol : m_symbols) {
                subscriptionMessage["channels"].push_back({
                    {"name", "ticker"},
                    {"product_ids", {symbol}}
                    });
            }
        }
        else if (m_market.find("bybit") != std::string::npos) {
            subscriptionMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : m_symbols) {
                subscriptionMessage["args"].push_back("orderbook.1." + symbol);
            }
        }
        else if (m_market.find("okx") != std::string::npos) {
            subscriptionMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : m_symbols) {
                subscriptionMessage["args"].push_back({
                    {"channel", "books"},
                    {"instId", symbol}
                    });
            }
        }
        else if (m_market.find("bitget") != std::string::npos) {
            subscriptionMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : m_symbols) {
                subscriptionMessage["args"].push_back({
                    {"instType", "sp"},
                    {"channel", "books5"},
                    {"instId", symbol}
                    });
            }
        }
        wsppClient.send(hdl, subscriptionMessage.dump(), websocketpp::frame::opcode::text);
        std::cout << "Subscription message sent: " << subscriptionMessage.dump() << std::endl;
    }
    catch (const websocketpp::exception& e) {
        std::cerr << "Failed to send subscription message: " << e.what() << std::endl;
    }
}

void CCryptoSocket::_ReadMessage(WSPPClient::message_ptr msg)
{
    try {
        std::string rawMessage = msg->get_payload();
        std::cout << "Raw : " << rawMessage << "\n";

        json jsonData = json::parse(rawMessage);
        m_pClientPanel->ReceivevData(rawMessage.c_str());

      //  if (m_market.find("bybit") != std::string::npos) {
      //      if (jsonData.contains("topic") && jsonData["topic"] == "orderbook") {
      //          std::string type = jsonData["type"].get<std::string>();
      //          std::string symbol = jsonData["symbol"].get<std::string>();
      //          std::string bidPrice = jsonData["data"]["bids"][0][0].get<std::string>();
      //          std::string bidQuantity = jsonData["data"]["bids"][0][1].get<std::string>();
      //          std::string askPrice = jsonData["data"]["asks"][0][0].get<std::string>();
      //          std::string askQuantity = jsonData["data"]["asks"][0][1].get<std::string>();

      //          std::string dataSet = "Type: " + type + "\n"
      //              + "Symbol: " + symbol + "\n"
      //              + "Bid Price: " + bidPrice + "\n"
      //              + "Bid Quantity: " + bidQuantity + "\n"
      //              + "Ask Price: " + askPrice + "\n"
      //              + "Ask Quantity: " + askQuantity + "\n";
      //          
      //          m_pClientPanel->SetData(dataSet.c_str());
      //      }
      //  }
      //  else {
		    //m_pClientPanel->SetData(rawMessage.c_str());
      //  }
    }
    catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }
}












//json jsonData = json::parse(msg->get_payload());
//if (m_market.find("binance") != std::string::npos) {}
//else if (m_market.find("coinbase") != std::string::npos) {}
//else if (m_market.find("bybit") != std::string::npos) {}
//else if (m_market.find("okx") != std::string::npos) {}
//else if (m_market.find("bitget") != std::string::npos) {}
//else if (m_market.find("gateio") != std::string::npos) {}
//else if (m_market.find("okx") != std::string::npos) {}
//else if (m_market.find("bitget") != std::string::npos) {}
//else if (m_market.find("gateio") != std::string::npos) {}