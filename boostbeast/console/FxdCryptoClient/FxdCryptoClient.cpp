#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ssl.hpp>

#include <nlohmann/json.hpp>


using json = nlohmann::json;

std::atomic<bool> stop_requested = false;
std::thread read_thread;

class CCryptoClient {

private:
    boost::asio::io_context ioc;
    boost::asio::ssl::context ssl_ctx;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::asio::ip::tcp::socket>> ws;

    std::string host;
    std::string port;

    std::string uri;
    std::vector<std::string> symbols;

public:
    CCryptoClient()
        : ssl_ctx(boost::asio::ssl::context::tlsv12_client), ws(ioc, ssl_ctx)
    {
    }

    ~CCryptoClient()
    {
        _Stop();
    }


    void _Run() {
        std::thread timerThread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Timer expired. Stopping client..." << std::endl;
            this->_Stop();
            });
        _Connect();
        _Subscribe();
        read_thread = std::thread(&CCryptoClient::_ReadMessage, this);
        timerThread.join();
    }

    void _Stop() {
        stop_requested = true;
        if (read_thread.joinable()) {
            read_thread.join();
        }
    }

private:
    void _Connect() {
        try {
            boost::asio::ip::tcp::resolver resolver(ioc);
            auto const results = resolver.resolve(host, port);

            boost::asio::connect(ws.next_layer().next_layer(), results.begin(), results.end());

            ws.next_layer().handshake(boost::asio::ssl::stream_base::client);
            ws.handshake(host, uri);
            //ws.handshake(host + ":" + port, uri);
            std::cout << "Connected to " << host << ":" << port << uri << std::endl;
        }
        catch (const boost::system::system_error& e) {
            std::cerr << "Connection error: " << e.what() << "\n";
            throw;
        }
    }

    void _Subscribe() {
        json subsMessage;
        if (host.find("binance") != std::string::npos) {
            subsMessage = {
                {"method", "SUBSCRIBE"},
                {"params", json::array()},
                {"id", 1}
            };
            for (const auto& symbol : symbols) {
                subsMessage["params"].push_back(symbol + "@depth");
            }

        }
        else if (uri.find("coinbase") != std::string::npos) {
            subsMessage = {
                {"type", "subscribe"},
                {"channels", json::array()}
            };
            for (const auto& symbol : symbols) {
                subsMessage["channels"].push_back({
                    {"name", "ticker"},
                    {"product_ids", {symbol}}
                    });
            }
        }
        else if (uri.find("bybit") != std::string::npos) {
            subsMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : symbols) {
                subsMessage["args"].push_back("orderbook.1." + symbol);
            }
        }
        else if (uri.find("okx") != std::string::npos) {
            subsMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : symbols) {
                subsMessage["args"].push_back({
                    {"channel", "books"},
                    {"instId", symbol}
                    });
            }
        }
        else if (uri.find("bitget") != std::string::npos) {
            subsMessage = {
                {"op", "subscribe"},
                {"args", json::array()}
            };
            for (const auto& symbol : symbols) {
                subsMessage["args"].push_back({
                    {"instType", "sp"},
                    {"channel", "books5"},
                    {"instId", symbol}
                    });
            };
        }
        ws.write(boost::asio::buffer(subsMessage.dump()));
        std::cout << "Subscription message sent: " << subsMessage.dump() << std::endl;
    }

    void _ReadMessage() {
        boost::beast::flat_buffer buffer;

        while (!stop_requested && ws.is_open()) {
            ws.read(buffer);
            std::string rawMessage = boost::beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size());
            json jsonData = json::parse(rawMessage);
            std::cout << "Received: " << jsonData.dump() << std::endl;
        }
    }

public:
    void _SetMarket(const std::string& market) {
        if (market == "a") {
            host = "stream.binance.com";
            port = "9443";
            uri = "/ws/stream";
        }
        else if (market == "b") {
            host = "ws-feed.exchange.coinbase.com";
            port = "443";
            uri = "/";
        }
        else if (market == "c") {
            host = "stream.bybit.com";
            port = "443";
            uri = "/v5/public/spot";
        }
        else if (market == "d") {
            host = "ws.okx.com";
            port = "8443";
            uri = "/ws/v5/public";
        }
        else if (market == "e") {
            host = "ws.bitget.com";
            port = "443";
            uri = "/spot/v1/stream";
        }
        else {
            throw std::invalid_argument("Invalid market.");
        }
    }

    void _SetSymbol(const std::string& input) {
        symbols.clear();
        for (char ch : input) {
            if (ch == ' ') continue;
            int option = ch - '0';
            std::string symbol;

            if (host.find("binance") != std::string::npos) {
                switch (option) {
                case 1: symbol = "btcusdt"; break;
                case 2: symbol = "ethusdt"; break;
                case 3: symbol = "xrpusdt"; break;
                case 4: symbol = "eosusdt"; break;
                case 5: symbol = "trxusdt"; break;
                case 6: symbol = "usdcusdt"; break;
                case 7: symbol = "bnbusdt"; break;
                default: std::cerr << "Invalid option: " << ch << std::endl; continue;
                }
            }
            else if (host.find("coinbase") != std::string::npos) {
                switch (option) {
                case 1: symbol = "BTC-USDT"; break;
                case 2: symbol = "ETH-USDT"; break;
                case 3: symbol = "XRP-USDT"; break;
                case 4: symbol = "EOS-USDT"; break;
                case 5: symbol = "BTC-USD"; break;
                case 6: symbol = "ETH-USD"; break;
                case 7: symbol = "XRP-USD"; break;
                case 8: symbol = "EOS-USD"; break;
                default: std::cerr << "Invalid option: " << ch << std::endl; continue;
                }
            }
            else if (host.find("bybit") != std::string::npos) {
                switch (option) {
                case 1: symbol = "BTCUSDT"; break;
                case 2: symbol = "ETHUSDT"; break;
                case 3: symbol = "XRPUSDT"; break;
                case 4: symbol = "EOSUSDT"; break;
                case 5: symbol = "TRXUSDT"; break;
                case 6: symbol = "USDCUSDT"; break;
                default: std::cerr << "Invalid option: " << ch << std::endl; continue;
                }
            }
            else if (host.find("okx") != std::string::npos) {
                switch (option) {
                case 1: symbol = "BTC-USDT"; break;
                case 2: symbol = "ETH-USDT"; break;
                case 3: symbol = "XRP-USDT"; break;
                case 4: symbol = "EOS-USDT"; break;
                case 5: symbol = "TRX-USDT"; break;
                case 6: symbol = "USDC-USDT"; break;
                default: std::cerr << "Invalid option: " << ch << std::endl; continue;
                }
            }
            else if (host.find("bitget") != std::string::npos) {
                switch (option) {
                case 1: symbol = "BTCUSDT"; break;
                case 2: symbol = "ETHUSDT"; break;
                case 3: symbol = "XRPUSDT"; break;
                case 4: symbol = "EOSUSDT"; break;
                case 5: symbol = "TRXUSDT"; break;
                case 6: symbol = "USDCUSDT"; break;
                case 7: symbol = "BTCUSD"; break;
                case 8: symbol = "ETHUSD"; break;
                case 9: symbol = "XRPUSD"; break;
                case 10: symbol = "EOSUSD"; break;
                case 11: symbol = "TRXUSD"; break;
                case 12: symbol = "USDCUSD"; break;
                default: std::cerr << "Invalid option: " << ch << std::endl; continue;
                }
            }

            if (!symbol.empty()) {
                symbols.push_back(symbol);
            }
        }
    }
};

int main() {
    try {
        std::string market;
        std::cout << "Binance [a], Coinbase [b], Bybit [c], Okx [d], Bitget [e]: ";
        std::cin >> market;

        CCryptoClient* pCryptoClient = new CCryptoClient();
		pCryptoClient->_SetMarket(market);

        std::string input;
        if (market == "a") {
            std::cout << "BTCUSDT [1], ETHUSDT [2], XRPUSDT [3], EOSUSDT [4], TRXUSDT [5],  USDCUSDT[6], BNBUSDT [7]: ";
        }
        else if (market == "b") {
            std::cout << "BTCUSDT [1], ETHUSDT [2], XRPUSDT [3], EOSUSDT [4], BTCUSD [5], ETHUSD [6], XRPUSD [7], EOSUSD [8]: ";
        }
        else if (market == "c") {
            std::cout << "BTCUSDT [1], ETHUSDT [2], XRPUSDT [3], EOSUSDT [4], TRXUSDT [5],  USDCUSDT[6]: ";
        }
        else if (market == "d") {
            std::cout << "BTCUSDT [1], ETHUSDT [2], XRPUSDT [3], EOSUSDT [4], TRXUSDT [5],  USDCUSDT[6]: ";
        }
        else if (market == "e") {
            std::cout << "BTCUSDT [1], ETHUSDT [2], XRPUSDT [3], EOSUSDT [4], TRXUSDT [5],  USDCUSDT[6], BTCUSD [7], ETHUSD [8], XRPUSD[9], EOSUSD[10], TRXUSD[11], USDCUSD[12]: ";
        }
        std::cin >> input;

        pCryptoClient->_SetSymbol(input);

        std::thread runThread([pCryptoClient]() {
            pCryptoClient->_Run();
        });
        
		runThread.join();

		delete pCryptoClient;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
