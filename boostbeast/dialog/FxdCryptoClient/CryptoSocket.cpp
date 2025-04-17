// CryptoSocket.cpp: 구현 파일
//

#include "pch.h"
#include "FxdCryptoClient.h"
#include "CryptoSocket.h"


// CCryptoSocket

IMPLEMENT_DYNAMIC(CCryptoSocket, CWnd)

CCryptoSocket::CCryptoSocket()
    : ssl_ctx(boost::asio::ssl::context::tlsv12_client), ws(ioc, ssl_ctx)

{

}

CCryptoSocket::~CCryptoSocket()
{
    _Stop();
}

BEGIN_MESSAGE_MAP(CCryptoSocket, CWnd)
END_MESSAGE_MAP()


// CCryptoSocket 메시지 처리기
void CCryptoSocket::_SetCallback(const std::function<void(const std::string&)>& callback)
{
	onReadCallback = callback;
}

void CCryptoSocket::_SetMarket(const std::string& market)
{
}

void CCryptoSocket::_SetSymbols(const std::string& input)
{
}

void CCryptoSocket::_Run()
{
}

void CCryptoSocket::_Stop()
{
}

void CCryptoSocket::_Connect()
{
}

void CCryptoSocket::_Subscribe()
{
}

void CCryptoSocket::_ReadMessages()
{
}
