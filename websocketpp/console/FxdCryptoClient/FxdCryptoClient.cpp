#include <iostream>
#include <string>
#include <vector>

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WSPPClient;
using json = nlohmann::json;

class CryptoClient {
private:
    WSPPClient wsppClient;

    std::string uri;
    std::vector<std::string> symbols;

public:
    CryptoClient(const std::string& market) {
        if (market == "a") {
            uri = "wss://stream.binance.com:9443/ws/stream";
        }
        else if (market == "b") {
            uri = "wss://ws-feed.exchange.coinbase.com";
        }
        else if (market == "c") {
            uri = "wss://stream.bybit.com/v5/public/spot";
        }
        else if (market == "d") {
            uri = "wss://ws.okx.com:8443/ws/v5/public";
        }
        else if (market == "e") {
            uri = "wss://ws.bitget.com/spot/v1/stream";
        }
        else {
            throw std::invalid_argument("Invalid market.");
        }
    }

    //  ~CryptoClient() {
          //wsppClient.stop();
    //  }

    void _Run() {
        _Init();
        _Connect();
        wsppClient.run();
    }

private:
    void _Init() {
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

    void _Connect() {
        websocketpp::lib::error_code ec;
        auto connection = wsppClient.get_connection(uri, ec);
        if (ec) {
            throw std::runtime_error("Connection failed: " + ec.message());
        }
        wsppClient.connect(connection);
    }

    void _Subscribe(websocketpp::connection_hdl hdl) {
        try {
            json subscriptionMessage;

            if (uri.find("binance") != std::string::npos) {
                subscriptionMessage = {
                    {"method", "SUBSCRIBE"},
                    {"params", json::array()},
                    {"id", 1}
                };
                for (const auto& symbol : symbols) {
                    subscriptionMessage["params"].push_back(symbol + "@depth");
                }
            }
            else if (uri.find("coinbase") != std::string::npos) {
                subscriptionMessage = {
                    {"type", "subscribe"},
                    {"channels", json::array()}
                };
                for (const auto& symbol : symbols) {
                    subscriptionMessage["channels"].push_back({
                        {"name", "ticker"},
                        {"product_ids", {symbol}}
                        });
                }
            }
            else if (uri.find("bybit") != std::string::npos) {
                subscriptionMessage = {
                    {"op", "subscribe"},
                    {"args", json::array()}
                };
                for (const auto& symbol : symbols) {
                    subscriptionMessage["args"].push_back("orderbook.1." + symbol);
                }
            }
            else if (uri.find("okx") != std::string::npos) {
                subscriptionMessage = {
                    {"op", "subscribe"},
                    {"args", json::array()}
                };
                for (const auto& symbol : symbols) {
                    subscriptionMessage["args"].push_back({
                        {"channel", "books"},
                        {"instId", symbol}
                        });
                }
            }
            else if (uri.find("bitget") != std::string::npos) {
                subscriptionMessage = {
                    {"op", "subscribe"},
                    {"args", json::array()}
                };
                for (const auto& symbol : symbols) {
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

    void _ReadMessage(WSPPClient::message_ptr msg) {
        try {
            std::string rawMessage = msg->get_payload();
            std::cout << "Raw : " << rawMessage << "\n";

            json jsonData = json::parse(msg->get_payload());
            if (uri.find("binance") != std::string::npos) {
                if (jsonData.contains("e") && jsonData["e"] == "depthUpdate") {
                    const auto& eventTimeMs = jsonData.contains("E") ? jsonData["E"].get<long long>() : 0;
                    std::time_t eventTimeSec = eventTimeMs / 1000;
                    std::tm tm{};
                    gmtime_s(&tm, &eventTimeSec);
                    char eventTime[20];
                    std::strftime(eventTime, sizeof(eventTime), "%Y-%m-%d %H:%M:%S", &tm);
                    const auto& symbol = jsonData.contains("s") ? jsonData["s"].get<std::string>() : "N/A";
                    const auto& bids = jsonData.contains("b") ? jsonData["b"] : json();
                    const auto& asks = jsonData.contains("a") ? jsonData["a"] : json();
                    const auto& firstUpdateId = jsonData.contains("U") ? jsonData["U"].get<long>() : 0;
                    const auto& lastUpdateId = jsonData.contains("u") ? jsonData["u"].get<long>() : 0;

                    //for (const auto& bid : bids) {
                    //    std::cout << "[" << eventTime << "] "
                    //        << "[Binance] "
                    //        << "[" << symbol << "] "
                    //        << "[Bid] "
                    //        << "[Price: " << bid[0].get<std::string>()
                    //        << ", Quantity: " << bid[1].get<std::string>() << "] "
                    //        << "[First Updatd Id: " << firstUpdateId << "] "
                    //        << "[Last Updatd Id: " << lastUpdateId << "] "
                    //        << "\n";
                    //}

                    //for (const auto& ask : asks) {
                    //    std::cout << "[" << eventTime << "] "
                    //        << "[Binance] "
                    //        << "[" << symbol << "] "
                    //        << "[Ask] "
                    //        << "[Price: " << ask[0].get<std::string>()
                    //        << ", Quantity: " << ask[1].get<std::string>() << "] "
                    //        << "[First Updatd Id: " << firstUpdateId << "] "
                    //        << "[Last Updatd Id: " << lastUpdateId << "] "
                    //        << "\n";
                    //}
                }
            }
            else if (uri.find("coinbase") != std::string::npos) {
                if (jsonData.contains("type") && jsonData["type"] == "ticker") {
                    const auto& isoTime = jsonData.contains("time") ? jsonData["time"].get<std::string>() : "N/A";
                    std::string time = isoTime;
                    size_t tPos = time.find('T'); if (tPos != std::string::npos) { time[tPos] = ' '; }
                    size_t zPos = time.find('Z'); if (zPos != std::string::npos) { time.erase(zPos); }
                    size_t dotPos = time.find('.'); if (dotPos != std::string::npos) { time.erase(dotPos); }
                    const auto& productId = jsonData["product_id"].get<std::string>();
                    const auto& side = jsonData.contains("side") ? jsonData["side"].get<std::string>() : "N/A";
                    const auto& bestBid = jsonData.contains("best_bid") ? jsonData["best_bid"].get<std::string>() : "N/A";
                    const auto& bestBidSize = jsonData.contains("best_bid_size") ? jsonData["best_bid_size"].get<std::string>() : "N/A";
                    const auto& bestAsk = jsonData.contains("best_ask") ? jsonData["best_ask"].get<std::string>() : "N/A";
                    const auto& bestAskSize = jsonData.contains("best_ask_size") ? jsonData["best_ask_size"].get<std::string>() : "N/A";
                    const auto& tradeId = jsonData.contains("trade_id") ? jsonData["trade_id"].get<int>() : -1;
                    const auto& sequence = jsonData.contains("sequence") ? jsonData["sequence"].get<uint64_t>() : 0;

                    //std::cout << "[" << time << "] "
                    //    << "[Coinbase] "
                    //    << "[" << productId << "] "
                    //    << "[" << side << "] "
                    //    << "[Highest Bid: " << bestBid
                    //    << ", Volume: " << bestBidSize << "] "
                    //    << "[Lowest Ask: " << bestAsk
                    //    << ", Volume: " << bestAskSize << "] "
                    //    << "[Trade ID: " << tradeId << "] "
                    //    << "[Sequence: " << sequence << "] "
                    //    << "\n";
                }
            }
            else if (uri.find("bybit") != std::string::npos) {
                if (jsonData.contains("topic") && jsonData["topic"] == "orderbook") {
                    const auto& symbol = jsonData.contains("symbol") ? jsonData["symbol"].get<std::string>() : "N/A";
                    //const auto& timestamp = jsonData.contains("timestamp_e6") ? jsonData["timestamp_e6"].get<long long>() : 0;
                    const auto& bids = jsonData.contains("bids") ? jsonData["bids"] : json();
                    const auto& asks = jsonData.contains("asks") ? jsonData["asks"] : json();
                    //for (const auto& bid : bids) {
                    //    std::cout << "[" << timestamp << "] "
                    //        << "[Bybit] "
                    //        << "[" << symbol << "] "
                    //        << "[Bid] "
                    //        << "[Price: " << bid["price"].get<std::string>()
                    //        << ", Quantity: " << bid["size"].get<std::string>() << "] "
                    //        << "\n";
                    //}
                    //for (const auto& ask : asks) {
                    //    std::cout << "[" << timestamp << "] "
                    //        << "[Bybit] "
                    //        << "[" << symbol << "] "
                    //        << "[Ask] "
                    //        << "[Price: " << ask["price"].get<std::string>()
                    //        << ", Quantity: " << ask["size"].get<std::string>() << "] "
                    //        << "\n";
                    //}

                    std::cout << "Bids (Price, Quantity):\n";
                    for (const auto& bid : bids) {
                        double price = std::stod(bid[0].get<std::string>());
                        double quantity = std::stod(bid[1].get<std::string>());
                        std::cout << "  " << price << ", " << quantity << "\n";
                    }

                    std::cout << "Asks (Price, Quantity):\n";
                    for (const auto& ask : asks) {
                        double price = std::stod(ask[0].get<std::string>());
                        double quantity = std::stod(ask[1].get<std::string>());
                        std::cout << "  " << price << ", " << quantity << "\n";
                    }
                }
                else if (uri.find("okx") != std::string::npos) {}
                else if (uri.find("bitget") != std::string::npos) {}
                else if (uri.find("gateio") != std::string::npos) {}
            }
            else if (uri.find("okx") != std::string::npos) {}
            else if (uri.find("bitget") != std::string::npos) {}
            else if (uri.find("gateio") != std::string::npos) {}
        }
        catch (const std::exception& e) {
            std::cerr << "Parsing error: " << e.what() << std::endl;
        }
    }

public:
    void _SetSymbols(const std::string& input) {
        symbols.clear();
        for (char ch : input) {
            if (ch == ' ') continue;
            int option = ch - '0';
            std::string symbol;

            if (uri.find("binance") != std::string::npos) {
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
            else if (uri.find("coinbase") != std::string::npos) {
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
            else if (uri.find("bybit") != std::string::npos) {
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
            else if (uri.find("okx") != std::string::npos) {
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
            else if (uri.find("bitget") != std::string::npos) {
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

        CryptoClient cryptoClient(market);

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

        cryptoClient._SetSymbols(input);
        cryptoClient._Run();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
