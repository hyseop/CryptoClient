// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

#define PCH_DELETE(x) if(x) delete x; x = nullptr;
#define WS_COMMON WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
#define PCH_CREATE_CHK(x, text, idc) if(x) x->Create(_T(text), WS_COMMON | BS_AUTOCHECKBOX /*| WS_BORDER*/, CRect(0, 0, 0, 0), this, idc);
#define PCH_CREATE_BTN(x, text, idc) if(x) x->Create(_T(text), WS_COMMON | BS_PUSHBUTTON /*| WS_BORDER*/, CRect(0, 0, 0, 0), this, idc);
#define PCH_CREATE_EDT(x, idc) if(x) x->Create(WS_COMMON | ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL /*| WS_BORDER*/, CRect(0, 0, 0, 0), this, idc);

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

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

#endif //PCH_H
