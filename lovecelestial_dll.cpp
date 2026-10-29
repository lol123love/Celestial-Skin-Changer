#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <psapi.h>
#include <urlmon.h>
#include <commdlg.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include "MinHook.h"
#include "ui_html.h"
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "comdlg32.lib")

static const char* kCredit = "made by lol123love for educational purposes and bcz love loves celestial";

static const char* kHookLog = "C:\\Users\\Public\\celestial_bypass.log";
static const char* kApiLog = "C:\\Users\\Public\\celestial_api.log";
static const int   kCdpPort = 9222;
static const int   kApiPortStart = 46321;
static const int   kApiPortEnd = 46340;
static const DWORD kSocketTimeoutHttp = 3000;
static const DWORD kSocketTimeoutWs = 10000;
static const DWORD kCdpEvalBudgetMs = 15000;

static HANDLE  g_hStopEvt = nullptr;
static int     g_apiPort = 0;
static CRITICAL_SECTION g_cdpLock;
static volatile LONG g_dllAttached = 0;
static std::string g_dllDir;
static volatile LONG g_forceUiReload = 0;

static void LogInit(const char* path) {
    HANDLE h = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) CloseHandle(h);
}

static void SetSocketTimeout(SOCKET s, DWORD ms) {
    DWORD tv = ms;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv, sizeof(tv));
}

static std::string GetCacheDir() {
    std::string base = g_dllDir.empty() ? "C:\\Users\\Public\\CelestialCache" : (g_dllDir + "\\cache");
    CreateDirectoryA(base.c_str(), NULL);
    return base;
}

static std::string GetImgCacheDir() {
    std::string dir = GetCacheDir() + "\\img";
    CreateDirectoryA(dir.c_str(), NULL);
    return dir;
}

static std::string LoadUiHtmlFromDisk() {
    return std::string(kCompiledUiHtml);
}

static void ExtractScriptsFromHtml(std::string& html, std::string& outScripts) {
    std::string lower = html;
    for (char& c : lower) c = (char)tolower((unsigned char)c);

    size_t pos = 0;
    while (true) {
        size_t startTag = lower.find("<script", pos);
        if (startTag == std::string::npos) break;
        size_t startClose = lower.find('>', startTag);
        if (startClose == std::string::npos) break;
        size_t endTag = lower.find("</script>", startClose + 1);
        if (endTag == std::string::npos) break;

        outScripts += "\n;\n" + html.substr(startClose + 1, endTag - (startClose + 1)) + "\n;\n";
        html.erase(startTag, (endTag + 9) - startTag);
        lower.erase(startTag, (endTag + 9) - startTag);
        pos = startTag;
    }
}

namespace mj {
    struct JVal {
        enum T { NUL, BOOL, NUM, STR, ARR, OBJ } t = NUL;
        bool b = false; double n = 0; std::string s;
        std::vector<JVal> arr;
        std::vector<std::pair<std::string, JVal>> obj;

        static JVal obj_() { JVal v; v.t = OBJ; return v; }
        static JVal arr_() { JVal v; v.t = ARR; return v; }
        static JVal str(const std::string& x) { JVal v; v.t = STR; v.s = x; return v; }
        static JVal num(double x) { JVal v; v.t = NUM; v.n = x; return v; }
        static JVal boo(bool x) { JVal v; v.t = BOOL; v.b = x; return v; }
        static JVal nul() { return JVal{}; }

        void set(const std::string& k, JVal v) {
            if (t != OBJ) { t = OBJ; obj.clear(); }
            for (auto& kv : obj) if (kv.first == k) { kv.second = std::move(v); return; }
            obj.emplace_back(k, std::move(v));
        }
        void push(JVal v) { if (t != ARR) { t = ARR; arr.clear(); } arr.push_back(std::move(v)); }
        const JVal* get(const std::string& k) const {
            if (t != OBJ) return nullptr;
            for (auto& kv : obj) if (kv.first == k) return &kv.second;
            return nullptr;
        }
        std::string asStr(const std::string& d = "") const { return t == STR ? s : d; }
        int64_t     asInt(int64_t d = 0) const { return t == NUM ? (int64_t)n : d; }
        bool        asBool(bool d = false) const { return t == BOOL ? b : d; }

        std::string dump() const { std::string o; ser(o); return o; }
        void ser(std::string& o) const {
            switch (t) {
            case NUL:  o += "null"; break;
            case BOOL: o += b ? "true" : "false"; break;
            case NUM: {
                char bb[40];
                if (n == (double)(int64_t)n) snprintf(bb, sizeof(bb), "%lld", (long long)n);
                else snprintf(bb, sizeof(bb), "%g", n);
                o += bb; break;
            }
            case STR: esc(o, s); break;
            case ARR: { o += '['; for (size_t i = 0; i < arr.size(); ++i) { if (i)o += ','; arr[i].ser(o); } o += ']'; break; }
            case OBJ: { o += '{'; for (size_t i = 0; i < obj.size(); ++i) { if (i)o += ','; esc(o, obj[i].first); o += ':'; obj[i].second.ser(o); } o += '}'; break; }
            }
        }
        static void esc(std::string& o, const std::string& s) {
            o += '"';
            for (char c : s) {
                switch (c) {
                case '"': o += "\\\""; break; case '\\': o += "\\\\"; break;
                case '\n': o += "\\n"; break;  case '\r': o += "\\r"; break;
                case '\t': o += "\\t"; break;
                default: if ((unsigned char)c < 0x20) { char b[8]; snprintf(b, sizeof(b), "\\u%04x", c); o += b; }
                       else o += c;
                }
            }
            o += '"';
        }
    };
    struct Parser {
        const char* p; const char* e; bool err = false;
        int depth = 0;
        void ws() { while (p < e && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) ++p; }
        bool eat(char c) { ws(); if (p < e && *p == c) { ++p; return true; } return false; }
        JVal parse() { return val(); }
        JVal val() {
            if (++depth > 200) { err = true; return {}; }
            ws(); if (p >= e) { err = true; depth--; return {}; }
            JVal res;
            if (*p == '{') res = ob();
            else if (*p == '[') res = ar();
            else if (*p == '"') res = JVal::str(str());
            else if (e - p >= 4 && !strncmp(p, "true", 4)) { p += 4; res = JVal::boo(true); }
            else if (e - p >= 5 && !strncmp(p, "false", 5)) { p += 5; res = JVal::boo(false); }
            else if (e - p >= 4 && !strncmp(p, "null", 4)) { p += 4; res = JVal::nul(); }
            else res = num();
            depth--;
            return res;
        }
        JVal ob() {
            JVal v = JVal::obj_(); ++p; ws();
            if (p < e && *p == '}') { ++p; return v; }
            while (p < e) {
                ws(); if (*p != '"') { err = true; return v; }
                std::string k = str(); ws();
                if (!eat(':')) { err = true; return v; }
                v.set(k, val()); ws();
                if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == '}') { ++p; return v; }
                err = true; return v;
            }
            err = true; return v;
        }
        JVal ar() {
            JVal v = JVal::arr_(); ++p; ws();
            if (p < e && *p == ']') { ++p; return v; }
            while (p < e) {
                v.push(val()); ws();
                if (p < e && *p == ',') { ++p; continue; }
                if (p < e && *p == ']') { ++p; return v; }
                err = true; return v;
            }
            err = true; return v;
        }
        std::string str() {
            std::string o; if (p >= e || *p != '"') { err = true; return o; } ++p;
            while (p < e && *p != '"') {
                char c = *p++;
                if (c == '\\' && p < e) {
                    char x = *p++;
                    switch (x) {
                    case '"': o += '"'; break; case '\\': o += '\\'; break; case '/': o += '/'; break;
                    case 'n': o += '\n'; break; case 'r': o += '\r'; break; case 't': o += '\t'; break;
                    case 'b': o += '\b'; break; case 'f': o += '\f'; break;
                    case 'u': {
                        if (p + 4 > e) { err = true; return o; } unsigned c2 = 0;
                        for (int i = 0; i < 4; ++i) { char h = *p++; c2 = c2 * 16 + (h >= 'a' ? h - 'a' + 10 : h >= 'A' ? h - 'A' + 10 : h - '0'); }
                        if (c2 < 0x80) o += (char)c2;
                        else if (c2 < 0x800) { o += (char)(0xC0 | (c2 >> 6)); o += (char)(0x80 | (c2 & 0x3F)); }
                        else { o += (char)(0xE0 | (c2 >> 12)); o += (char)(0x80 | ((c2 >> 6) & 0x3F)); o += (char)(0x80 | (c2 & 0x3F)); }
                        break;
                    }
                    default: err = true; return o;
                    }
                }
                else o += c;
            }
            if (p >= e || *p != '"') { err = true; return o; }
            ++p; return o;
        }
        JVal num() {
            const char* s = p;
            if (p < e && *p == '-') ++p;
            while (p < e && ((*p >= '0' && *p <= '9') || *p == '.' || *p == 'e' || *p == 'E' || *p == '+' || *p == '-')) ++p;
            if (p == s) { err = true; return {}; }
            JVal v; v.t = JVal::NUM; v.n = atof(std::string(s, p - s).c_str()); return v;
        }
    };
    static JVal parse(const std::string& s) { Parser pp{ s.data(), s.data() + s.size() }; return pp.parse(); }
}

static std::string B64Enc(const uint8_t* d, size_t n) {
    static const char* T = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string o; o.reserve(((n + 2) / 3) * 4);
    for (size_t i = 0; i < n; i += 3) {
        uint32_t v = d[i] << 16;
        if (i + 1 < n) v |= d[i + 1] << 8;
        if (i + 2 < n) v |= d[i + 2];
        o += T[(v >> 18) & 63]; o += T[(v >> 12) & 63];
        o += (i + 1 < n) ? T[(v >> 6) & 63] : '=';
        o += (i + 2 < n) ? T[v & 63] : '=';
    }
    return o;
}

struct WsClient {
    SOCKET s = INVALID_SOCKET;
    bool open = false;

    bool connect(const std::string& wsUrl) {
        std::string u = wsUrl;
        if (u.size() < 5 || u.compare(0, 5, "ws://") != 0) return false;
        u = u.substr(5);
        size_t slash = u.find('/');
        std::string hostport = (slash == std::string::npos) ? u : u.substr(0, slash);
        std::string path = (slash == std::string::npos) ? "/" : u.substr(slash);
        std::string host = hostport; int port = kCdpPort;
        size_t colon = hostport.find(':');
        if (colon != std::string::npos) { host = hostport.substr(0, colon); port = atoi(hostport.c_str() + colon + 1); }

        addrinfo hints{}, * res = nullptr;
        hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
        char portStr[16]; snprintf(portStr, sizeof(portStr), "%d", port);
        if (getaddrinfo(host.c_str(), portStr, &hints, &res) != 0) return false;
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s == INVALID_SOCKET) { freeaddrinfo(res); return false; }
        if (::connect(s, res->ai_addr, (int)res->ai_addrlen) != 0) {
            closesocket(s); s = INVALID_SOCKET; freeaddrinfo(res); return false;
        }
        freeaddrinfo(res);

        SetSocketTimeout(s, kSocketTimeoutWs);

        uint8_t keyRaw[16];
        for (int i = 0; i < 16; ++i) keyRaw[i] = (uint8_t)(rand() & 0xFF);
        std::string key = B64Enc(keyRaw, 16);
        std::string req =
            "GET " + path + " HTTP/1.1\r\nHost: " + host + ":" + portStr + "\r\n"
            "Upgrade: websocket\r\nConnection: Upgrade\r\n"
            "Sec-WebSocket-Key: " + key + "\r\nSec-WebSocket-Version: 13\r\n\r\n";
        send(s, req.c_str(), (int)req.size(), 0);

        std::string hdr; char b[1];
        while (hdr.find("\r\n\r\n") == std::string::npos) {
            int r = recv(s, b, 1, 0);
            if (r <= 0) { close(); return false; }
            hdr += b[0];
            if (hdr.size() > 8192) { close(); return false; }
        }
        if (hdr.find(" 101 ") == std::string::npos &&
            hdr.find(" 101\r") == std::string::npos) {
            close(); return false;
        }
        open = true;
        return true;
    }

    bool sendText(const std::string& t) {
        if (!open) return false;
        std::string frame;
        frame.push_back((char)0x81);
        size_t len = t.size();
        if (len < 126) frame.push_back((char)(0x80 | (uint8_t)len));
        else if (len < 65536) { frame.push_back((char)(0x80 | 126)); frame.push_back((char)((len >> 8) & 0xFF)); frame.push_back((char)(len & 0xFF)); }
        else { frame.push_back((char)(0x80 | 127)); for (int i = 7; i >= 0; --i) frame.push_back((char)((len >> (i * 8)) & 0xFF)); }
        uint8_t mk[4] = { (uint8_t)(rand() & 0xFF), (uint8_t)(rand() & 0xFF), (uint8_t)(rand() & 0xFF), (uint8_t)(rand() & 0xFF) };
        frame.append((char*)mk, 4);
        for (size_t i = 0; i < len; ++i) frame.push_back((char)(t[i] ^ mk[i % 4]));
        return send(s, frame.data(), (int)frame.size(), 0) == (int)frame.size();
    }

    bool recvExact(void* buf, size_t n) {
        size_t got = 0;
        while (got < n) {
            int r = recv(s, (char*)buf + got, (int)(n - got), 0);
            if (r <= 0) return false;
            got += (size_t)r;
        }
        return true;
    }

    std::string recvText() {
        std::string message;
        while (open) {
            uint8_t hdr[2];
            if (!recvExact(hdr, 2)) { open = false; return {}; }

            bool fin = (hdr[0] & 0x80) != 0;
            uint8_t opcode = hdr[0] & 0x0F;
            bool masked = (hdr[1] & 0x80) != 0;
            uint64_t len = hdr[1] & 0x7F;

            if (len == 126) {
                uint8_t b[2]; if (!recvExact(b, 2)) { open = false; return {}; }
                len = ((uint64_t)b[0] << 8) | b[1];
            }
            else if (len == 127) {
                uint8_t b[8]; if (!recvExact(b, 8)) { open = false; return {}; }
                len = 0; for (int i = 0; i < 8; ++i) len = (len << 8) | b[i];
            }

            if (len > 64 * 1024 * 1024) { open = false; return {}; }

            uint8_t mk[4] = {};
            if (masked && !recvExact(mk, 4)) { open = false; return {}; }

            std::string payload;
            if (len) {
                payload.resize((size_t)len);
                if (!recvExact(&payload[0], (size_t)len)) { open = false; return {}; }
                if (masked) for (size_t i = 0; i < len; ++i) payload[i] = (char)(payload[i] ^ mk[i % 4]);
            }

            if (opcode == 0x8) { open = false; return {}; }
            if (opcode == 0x9 || opcode == 0xA) continue;

            message += payload;
            if (fin) return message;
        }
        return {};
    }

    void close() {
        if (s != INVALID_SOCKET) { closesocket(s); s = INVALID_SOCKET; }
        open = false;
    }
};

static std::string HttpGet(int port, const char* path) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return {};
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons((u_short)port);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
    if (::connect(s, (sockaddr*)&a, sizeof(a)) != 0) { closesocket(s); return {}; }

    SetSocketTimeout(s, kSocketTimeoutHttp);

    char req[256];
    snprintf(req, sizeof(req), "GET %s HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n", path);
    send(s, req, (int)strlen(req), 0);

    std::string resp; char buf[4096]; int n;
    int expectedLen = -1;
    size_t hdrEnd = std::string::npos;

    while ((n = recv(s, buf, sizeof(buf), 0)) > 0) {
        resp.append(buf, n);
        if (hdrEnd == std::string::npos) {
            hdrEnd = resp.find("\r\n\r\n");
            if (hdrEnd != std::string::npos) {
                std::string h = resp.substr(0, hdrEnd);
                for (char& c : h) c = (char)tolower((unsigned char)c);
                size_t p = h.find("content-length:");
                if (p != std::string::npos) expectedLen = atoi(h.c_str() + p + 15);
            }
        }
        if (hdrEnd != std::string::npos && expectedLen >= 0) {
            if (resp.size() >= hdrEnd + 4 + expectedLen) break;
        }
    }
    closesocket(s);
    if (hdrEnd == std::string::npos) return {};
    return resp.substr(hdrEnd + 4);
}

static bool CdpAvailable() { return !HttpGet(kCdpPort, "/json/version").empty(); }

static std::string CdpFindWsUrl() {
    std::string body = HttpGet(kCdpPort, "/json");
    if (body.empty()) return {};
    mj::JVal arr = mj::parse(body);
    if (arr.t != mj::JVal::ARR) return {};
    for (auto& t : arr.arr) {
        if (t.get("type") && t.get("type")->asStr() == "page") {
            auto* url = t.get("url"); auto* wsu = t.get("webSocketDebuggerUrl");
            if (url && wsu && url->asStr().find("tauri.localhost") != std::string::npos) return wsu->asStr();
        }
    }
    for (auto& t : arr.arr) {
        auto* wsu = t.get("webSocketDebuggerUrl");
        if (wsu) return wsu->asStr();
    }
    return {};
}

static WsClient g_wsCache;
static int g_cdpId = 0;

static std::string CdpEvaluate(const std::string& jsExpr, bool returnByValue = true, int timeoutMs = (int)kCdpEvalBudgetMs) {
    if (!g_wsCache.open) {
        std::string wsUrl = CdpFindWsUrl();
        if (wsUrl.empty() || !g_wsCache.connect(wsUrl)) return {};
        g_wsCache.sendText("{\"id\":999998,\"method\":\"Page.enable\",\"params\":{}}");
        g_wsCache.sendText("{\"id\":999999,\"method\":\"Page.setBypassCSP\",\"params\":{\"enabled\":true}}");
    }

    int myId = ++g_cdpId;
    mj::JVal req = mj::JVal::obj_();
    req.set("id", mj::JVal::num(myId));
    req.set("method", mj::JVal::str("Runtime.evaluate"));

    mj::JVal params = mj::JVal::obj_();
    params.set("expression", mj::JVal::str(jsExpr));
    params.set("awaitPromise", mj::JVal::boo(true));
    params.set("returnByValue", mj::JVal::boo(returnByValue));
    req.set("params", std::move(params));

    if (!g_wsCache.sendText(req.dump())) {
        g_wsCache.close();
        return {};
    }

    DWORD start = GetTickCount64();
    while (GetTickCount64() - start < (DWORD)timeoutMs) {
        std::string frame = g_wsCache.recvText();
        if (frame.empty()) { g_wsCache.close(); return {}; }

        mj::JVal rsp = mj::parse(frame);
        auto* id = rsp.get("id");
        if (!id || id->asInt() != myId) continue;

        auto* resultPtr = rsp.get("result");
        if (!resultPtr) return frame;
        auto* inner = resultPtr->get("result");
        if (!inner) return frame;
        auto* val = inner->get("value");
        return val ? val->dump() : inner->dump();
    }

    g_wsCache.close();
    return {};
}

static HWND FindCelestialMainWindow() {
    HWND found = NULL;
    EnumWindows([](HWND hwnd, LPARAM lp) -> BOOL {
        DWORD wpid = 0;
        GetWindowThreadProcessId(hwnd, &wpid);
        if (wpid == GetCurrentProcessId() && GetWindow(hwnd, GW_OWNER) == NULL && IsWindowVisible(hwnd)) {
            *(HWND*)lp = hwnd;
            return FALSE;
        }
        return TRUE;
        }, (LPARAM)&found);
    return found;
}


static const char* kLightweightPollJs = R"JS(
(function() {
  if (!window.__TAURI_INTERNALS__ || !document.body) return 'WAITING';
  if (window.__CEL_WIN_CMD__) {
    var c = window.__CEL_WIN_CMD__;
    window.__CEL_WIN_CMD__ = '';
    return 'WIN_CMD:' + c;
  }
  if (window.__CEL_RELOAD_REQ__) {
    window.__CEL_RELOAD_REQ__ = false;
    return 'NEED_RELOAD';
  }
  if (document.body.id === 'celestinot-root' && !document.getElementById('root')) {
    return 'ALREADY_INJECTED';
  }
  return 'NEED_INJECT';
})()
)JS";

static std::string BuildUiInjectionJs() {
    std::string htmlContent = LoadUiHtmlFromDisk();
    std::string userScripts;
    ExtractScriptsFromHtml(htmlContent, userScripts);

    std::string escapedHtml = mj::JVal::str(htmlContent).dump();
    char portBuf[32];
    snprintf(portBuf, sizeof(portBuf), "%d", g_apiPort);

    std::string js = "(async function() {\n";
    js += "  window.__CEL_API_PORT__ = " + std::string(portBuf) + ";\n";
    js += "  const rawHtml = " + escapedHtml + ";\n";
    js += R"JS(
  const existingStyle = document.querySelector('style[nonce]');
  const nonce = existingStyle ? (existingStyle.nonce || existingStyle.getAttribute('nonce') || '') : '';

  document.querySelectorAll('link[rel="stylesheet"]').forEach(l => l.remove());
  document.querySelectorAll('style[data-celestinot]').forEach(s => s.remove());

  const temp = document.createElement('div');
  temp.innerHTML = rawHtml;

  let combinedCss = '';
  temp.querySelectorAll('style').forEach(st => {
    combinedCss += '\n' + st.textContent + '\n';
    st.remove();
  });

  let inlCount = 0;
  temp.querySelectorAll('[style]').forEach(el => {
    const cls = 'cel-inl-' + (++inlCount);
    combinedCss += `.${cls} { ${el.getAttribute('style')} }\n`;
    el.classList.add(cls);
    el.removeAttribute('style');
  });

  const masterStyle = document.createElement('style');
  if (nonce) masterStyle.setAttribute('nonce', nonce);
  masterStyle.setAttribute('data-celestinot', '1');
  masterStyle.textContent = combinedCss;
  document.head.appendChild(masterStyle);

  document.body.id = 'celestinot-root';
  document.body.className = '';
  document.body.innerHTML = temp.innerHTML;

  window.celApi = {
    invoke: async (cmd, args = {}) => {
      try {
        return await window.__TAURI_INTERNALS__.invoke(cmd, args);
      } catch (e) {
        return { ok: false, error: (e && e.message) ? e.message : String(e) };
      }
    },
    start: () => window.celApi.invoke('start_patcher', { config: {} }),
    stop:  () => window.celApi.invoke('stop_patcher', {}),
    toggle: (id, state) => window.celApi.invoke('toggle_mod', { modId: id, enabled: !!state }),
    importSkin: (path) => {
      const low = String(path).toLowerCase();
      const cmd = low.endsWith('.modpkg') ? 'import_modpkg' :
                  low.includes('.wad') ? 'import_wad_client' : 'import_fantome';
      return window.celApi.invoke(cmd, {
        path, filePath: path, fantomePath: path, modpkgPath: path, wadPath: path, folderPath: path
      });
    },
    reloadUi: () => { window.__CEL_RELOAD_REQ__ = true; }
  };
)JS";

    if (!userScripts.empty()) {
        js += "\n  try {\n" + userScripts + "\n  } catch (scrErr) { console.error('ui.html script error:', scrErr); }\n";
    }

    js += "\n  return 'INJECTED_SUCCESSFULLY';\n})()";
    return js;
}

static mj::JVal TauriInvoke(const std::string& command, const mj::JVal& args) {
    std::string js =
        "(async () => { try { const r = await window.__TAURI_INTERNALS__.invoke("
        + mj::JVal::str(command).dump() + ", " + args.dump() +
        "); return r; } catch (e) { return { ok: false, error: (e && e.message) ? e.message : String(e) }; } })()";

    BOOL entered = FALSE;
    for (int i = 0; i < 100 && !entered; ++i) {
        entered = TryEnterCriticalSection(&g_cdpLock);
        if (!entered) Sleep(20);
    }
    if (!entered) {
        mj::JVal err = mj::JVal::obj_();
        err.set("ok", mj::JVal::boo(false));
        err.set("error", mj::JVal::str("CDP busy"));
        return err;
    }

    std::string raw;
    try { raw = CdpEvaluate(js); }
    catch (...) {}
    LeaveCriticalSection(&g_cdpLock);

    if (raw.empty()) {
        mj::JVal err = mj::JVal::obj_();
        err.set("ok", mj::JVal::boo(false));
        err.set("error", mj::JVal::str("CDP unavailable"));
        return err;
    }
    return mj::parse(raw);
}

static mj::JVal CmdImport(const std::string& path);
static mj::JVal CmdToggle(const std::string& target, bool enable);
static mj::JVal CmdStartPatcher();
static mj::JVal CmdStopPatcher();

static void NotifyUiEvent(const std::string& evType, const std::string& msg) {
    EnterCriticalSection(&g_cdpLock);
    std::string js = "if(window.__CEL_ON_NATIVE_EVENT__) window.__CEL_ON_NATIVE_EVENT__("
        + mj::JVal::str(evType).dump() + "," + mj::JVal::str(msg).dump() + ");";
    CdpEvaluate(js, false, 3000);
    LeaveCriticalSection(&g_cdpLock);
}

static void EnsureDirExists(const std::string& dirPath) {
    std::string cur;
    for (size_t i = 0; i < dirPath.size(); ++i) {
        char c = dirPath[i];
        cur += c;
        if (c == '\\' || c == '/' || i + 1 == dirPath.size()) {
            if (cur.size() > 3) CreateDirectoryA(cur.c_str(), NULL);
        }
    }
}


static DWORD WINAPI SyncAllRepoThread(LPVOID) {
    std::string baseDir = GetCacheDir();
    std::string zipPath = baseDir + "\\LeagueSkins-main.zip";

    NotifyUiEvent("SYNC_PROGRESS", "⏳ DOWNLOADING REPO ZIP...");

    DeleteFileA(zipPath.c_str());

    std::string curlCmd = "curl.exe -L -f -s \"https://codeload.github.com/Alban1911/LeagueSkins/zip/refs/heads/main\" -o \"" + zipPath + "\"";
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    if (CreateProcessA(NULL, &curlCmd[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        URLDownloadToFileA(NULL, "https://codeload.github.com/Alban1911/LeagueSkins/zip/refs/heads/main", zipPath.c_str(), 0, NULL);
    }

    if (GetFileAttributesA(zipPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        NotifyUiEvent("SYNC_PROGRESS", "✗ REPO DOWNLOAD FAILED");
        return 0;
    }

    NotifyUiEvent("SYNC_PROGRESS", "⏳ EXTRACTING & OVERWRITING SKINS...");

    std::string tarCmd = "tar.exe --strip-components=1 -xf \"" + zipPath + "\" -C \"" + baseDir + "\"";
    STARTUPINFOA si2 = { sizeof(si2) };
    PROCESS_INFORMATION pi2 = {};
    if (CreateProcessA(NULL, &tarCmd[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si2, &pi2)) {
        WaitForSingleObject(pi2.hProcess, INFINITE);
        CloseHandle(pi2.hProcess);
        CloseHandle(pi2.hThread);
    }

    DeleteFileA(zipPath.c_str());
    NotifyUiEvent("SYNC_DONE", "✓ FULL REPO SYNCED TO cache\\skins! ALL SKINS READY FOR INSTANT USE.");
    return 0;
}

struct SkinDlParams {
    std::string cid;
    std::string baseId;
    std::string chromaId;
    std::string champName;
};


static DWORD WINAPI DownloadAndActivateSkinThread(LPVOID param) {
    SkinDlParams p = *(SkinDlParams*)param;
    delete (SkinDlParams*)param;

    std::string baseDir = GetCacheDir();
    std::string targetId = (p.chromaId != "-1" && !p.chromaId.empty()) ? p.chromaId : p.baseId;

 
    std::string relFolder = "skins\\" + p.cid + "\\" + p.baseId;
    std::string urlSub = p.cid + "/" + p.baseId + "/";
    if (p.chromaId != "-1" && !p.chromaId.empty()) {
        relFolder += "\\" + p.chromaId;
        urlSub += p.chromaId + "/";
    }

    std::string skinFolder = baseDir + "\\" + relFolder;
    EnsureDirExists(skinFolder);

    std::string destFantome = skinFolder + "\\" + targetId + ".fantome";
    std::string destZip = skinFolder + "\\" + targetId + ".zip";
    std::string legacyFantome = baseDir + "\\LeagueSkins-main\\" + relFolder + "\\" + targetId + ".fantome";
    std::string legacyZip = baseDir + "\\LeagueSkins-main\\" + relFolder + "\\" + targetId + ".zip";

    std::string readyFile;

    if (GetFileAttributesA(destFantome.c_str()) != INVALID_FILE_ATTRIBUTES) {
        readyFile = destFantome;
    }
    else if (GetFileAttributesA(destZip.c_str()) != INVALID_FILE_ATTRIBUTES) {
        CopyFileA(destZip.c_str(), destFantome.c_str(), FALSE);
        readyFile = destFantome;
    }
    else if (GetFileAttributesA(legacyFantome.c_str()) != INVALID_FILE_ATTRIBUTES) {
        CopyFileA(legacyFantome.c_str(), destFantome.c_str(), FALSE);
        readyFile = destFantome;
    }
    else if (GetFileAttributesA(legacyZip.c_str()) != INVALID_FILE_ATTRIBUTES) {
        CopyFileA(legacyZip.c_str(), destFantome.c_str(), FALSE);
        readyFile = destFantome;
    }

    if (readyFile.empty()) {
        std::vector<std::string> urls = {
            "https://raw.githubusercontent.com/Alban1911/LeagueSkins/main/skins/" + urlSub + targetId + ".fantome",
            "https://raw.githubusercontent.com/Alban1911/LeagueSkins/main/skins/" + urlSub + targetId + ".zip"
        };
        for (const auto& u : urls) {
            if (URLDownloadToFileA(NULL, u.c_str(), destFantome.c_str(), 0, NULL) == S_OK) {
                if (GetFileAttributesA(destFantome.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    readyFile = destFantome;
                    break;
                }
            }
        }
    }

    if (readyFile.empty()) {
        NotifyUiEvent("ERROR", "Skin " + targetId + " not found in cloud repository.");
        return 0;
    }

    CmdStopPatcher();
    Sleep(200);

    mj::JVal libResp = TauriInvoke("get_mod_library", mj::JVal::obj_());
    auto* libVal = libResp.get("value");
    if (libVal && libVal->t == mj::JVal::OBJ) {
        auto* mods = libVal->get("mods");
        if (mods && mods->t == mj::JVal::ARR) {
            for (auto& m : mods->arr) {
                auto* ch = m.get("champion");
                auto* en = m.get("enabled");
                auto* id = m.get("id");
                if (ch && en && id && en->asBool()) {
                    std::string c1 = ch->asStr(), c2 = p.champName;
                    for (char& c : c1) c = (char)tolower((unsigned char)c);
                    for (char& c : c2) c = (char)tolower((unsigned char)c);
                    if (c1 == c2) {
                        mj::JVal targs = mj::JVal::obj_();
                        targs.set("modId", mj::JVal::str(id->asStr()));
                        targs.set("enabled", mj::JVal::boo(false));
                        TauriInvoke("toggle_mod", targs);
                    }
                }
            }
        }
    }

    mj::JVal impResp = CmdImport(readyFile);
    auto* innerResp = impResp.get("response");
    if (innerResp) {
        auto* val = innerResp->get("value") ? innerResp->get("value") : innerResp;
        if (val && val->get("id")) {
            CmdToggle(val->get("id")->asStr(), true);
        }
    }

    Sleep(200);
    CmdStartPatcher();
    NotifyUiEvent("SKIN_READY", "Activated " + p.champName + " (" + targetId + ") & Restarted Patcher!");
    return 0;
}

static DWORD WINAPI CustomUiThread(LPVOID) {
    while (WaitForSingleObject(g_hStopEvt, 350) == WAIT_TIMEOUT) {
        if (!CdpAvailable()) continue;

        bool force = (InterlockedExchange(&g_forceUiReload, 0) != 0);

        BOOL entered = TryEnterCriticalSection(&g_cdpLock);
        if (!entered) continue;

        std::string res = force ? "NEED_RELOAD" : CdpEvaluate(kLightweightPollJs, true, 2000);
        if (res.find("NEED_INJECT") != std::string::npos || res.find("NEED_RELOAD") != std::string::npos) {
            CdpEvaluate(BuildUiInjectionJs(), true, 5000);
            LeaveCriticalSection(&g_cdpLock);
            continue;
        }
        else if (res.find("WIN_CMD:CLOSE") != std::string::npos) {
            LeaveCriticalSection(&g_cdpLock);
            ExitProcess(0);
        }
        else if (res.find("WIN_CMD:MINIMIZE") != std::string::npos) {
            HWND hw = FindCelestialMainWindow();
            if (hw) ShowWindow(hw, SW_MINIMIZE);
        }
        else if (res.find("WIN_CMD:MAXIMIZE") != std::string::npos) {
            HWND hw = FindCelestialMainWindow();
            if (hw) ShowWindow(hw, IsZoomed(hw) ? SW_RESTORE : SW_MAXIMIZE);
        }
        else if (res.find("WIN_CMD:OPEN_FILE_DIALOG") != std::string::npos) {
            LeaveCriticalSection(&g_cdpLock);
            char fileBuf[MAX_PATH] = {};
            OPENFILENAMEA ofn = {};
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = FindCelestialMainWindow();
            ofn.lpstrFilter = "Skin Packages (*.fantome;*.modpkg;*.zip;*.wad.client)\0*.fantome;*.modpkg;*.zip;*.wad.client;*.wad\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = fileBuf;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
            if (GetOpenFileNameA(&ofn) && fileBuf[0]) {
                CmdImport(fileBuf);
                EnterCriticalSection(&g_cdpLock);
                CdpEvaluate("if(window.__CEL_ON_FILE_DROPPED__) window.__CEL_ON_FILE_DROPPED__(" + mj::JVal::str(fileBuf).dump() + ");", false, 3000);
                LeaveCriticalSection(&g_cdpLock);
            }
            continue;
        }
        else if (res.find("WIN_CMD:SYNC_ALL_REPO") != std::string::npos) {
            LeaveCriticalSection(&g_cdpLock);
            HANDLE th = CreateThread(NULL, 0, SyncAllRepoThread, NULL, 0, NULL);
            if (th) CloseHandle(th);
            continue;
        }
        else if (res.find("WIN_CMD:DOWNLOAD_SKIN|") != std::string::npos) {
            LeaveCriticalSection(&g_cdpLock);
            size_t pos = res.find("WIN_CMD:DOWNLOAD_SKIN|") + 22;
            std::string tail = res.substr(pos);
            if (!tail.empty() && tail.back() == '"') tail.pop_back();

            std::vector<std::string> parts;
            size_t start = 0, bar;
            while ((bar = tail.find('|', start)) != std::string::npos) {
                parts.push_back(tail.substr(start, bar - start));
                start = bar + 1;
            }
            parts.push_back(tail.substr(start));

            if (parts.size() >= 4) {
                SkinDlParams* dp = new SkinDlParams{ parts[0], parts[1], parts[2], parts[3] };
                HANDLE th = CreateThread(NULL, 0, DownloadAndActivateSkinThread, dp, 0, NULL);
                if (th) CloseHandle(th);
            }
            continue;
        }

        LeaveCriticalSection(&g_cdpLock);
    }
    return 0;
}

static bool VerifyPrologue(uintptr_t addr, const uint8_t* pat, size_t n) {
    __try { for (size_t i = 0; i < n; ++i) if (*(uint8_t*)(addr + i) != pat[i]) return false; return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}
static const uint8_t PRO_BLOCK[16] = { 0x55,0x41,0x57,0x41,0x56,0x41,0x55,0x41,0x54,0x56,0x57,0x53,0x48,0x81,0xEC,0x48 };
static const uint8_t PRO_HASH[16] = { 0x55,0x41,0x57,0x41,0x56,0x41,0x55,0x41,0x54,0x56,0x57,0x53,0x48,0x81,0xEC,0x48 };
static const uint8_t PRO_HEUR[16] = { 0x55,0x41,0x57,0x41,0x56,0x41,0x55,0x41,0x54,0x56,0x57,0x53,0x48,0x81,0xEC,0xB8 };

typedef uint64_t* (__fastcall* tBlock)(uint64_t*, int64_t, int64_t);
static tBlock g_origBlock = nullptr;
static LONG g_cBlock = 0; static bool g_rBlock = false; static const char* g_mBlock = "(none)";
static uint64_t* __fastcall HookBlock(uint64_t* a1, int64_t, int64_t) {
    InterlockedIncrement(&g_cBlock);
    if (a1) { a1[0] = 0x8000000000000000ULL; a1[1] = 0; a1[2] = 0; }
    return a1;
}
typedef uint64_t(__fastcall* tHash)(uint64_t*, int64_t, int64_t, int64_t, int64_t);
static tHash g_origHash = nullptr;
static LONG g_cHash = 0; static bool g_rHash = false; static const char* g_mHash = "(none)";
static uint64_t __fastcall HookHash(uint64_t* a1, int64_t, int64_t, int64_t, int64_t) {
    InterlockedIncrement(&g_cHash);
    if (a1) { a1[0] = 0x8000000000000000ULL; a1[1] = 0; a1[2] = 0; }
    return 0x8000000000000000ULL;
}
typedef void* (__fastcall* tHeur)(void*, int64_t, int64_t, int64_t, int64_t, uint64_t, int64_t, int64_t);
static tHeur g_origHeur = nullptr;
static LONG g_cHeur = 0; static bool g_rHeur = false; static const char* g_mHeur = "(none)";
static void* __fastcall HookHeur(void* a1, int64_t, int64_t, int64_t, int64_t, uint64_t, int64_t, int64_t) {
    InterlockedIncrement(&g_cHeur);
    if (a1) { ((uint64_t*)a1)[0] = 0x8000000000000000ULL; ((uint8_t*)a1)[8] = 0; }
    return a1;
}

static BOOL CALLBACK HideOneWindowInternal(HWND hwnd, LPARAM lp) {
    DWORD pid = (DWORD)lp; DWORD wpid = 0;
    GetWindowThreadProcessId(hwnd, &wpid);
    if (wpid != pid) return TRUE;
    if (GetWindow(hwnd, GW_OWNER) != NULL) return TRUE;
    if (!IsWindowVisible(hwnd)) return TRUE;
    ShowWindow(hwnd, SW_HIDE);
    return TRUE;
}
static DWORD WINAPI HideThread(LPVOID) {
    DWORD pid = GetCurrentProcessId();
    while (WaitForSingleObject(g_hStopEvt, 100) == WAIT_TIMEOUT)
        EnumWindows(HideOneWindowInternal, (LPARAM)pid);
    return 0;
}

static bool WritePortFile(const char* path, int port) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d\n", port);
    std::string tmp = std::string(path) + ".tmp";
    HANDLE h = CreateFileA(tmp.c_str(), GENERIC_WRITE, 0, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return false;
    DWORD w = 0;
    BOOL ok = WriteFile(h, buf, (DWORD)strlen(buf), &w, NULL);
    CloseHandle(h);
    if (!ok) { DeleteFileA(tmp.c_str()); return false; }
    return MoveFileExA(tmp.c_str(), path,
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}
static void BroadcastPort(int port) {
    HKEY hk;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\LoveCelestial", 0, NULL, 0,
        KEY_WRITE, NULL, &hk, NULL) == ERROR_SUCCESS) {
        DWORD v = (DWORD)port;
        RegSetValueExA(hk, "ApiPort", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
        RegCloseKey(hk);
    }
    char localApp[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", localApp, MAX_PATH) > 0) {
        std::string dir = std::string(localApp) + "\\LoveCelestial";
        CreateDirectoryA(dir.c_str(), NULL);
        std::string p = dir + "\\port.txt";
        WritePortFile(p.c_str(), port);
    }
    WritePortFile("C:\\Users\\Public\\LoveCelestial.port", port);
}
static void ClearPortBroadcast() {
    HKEY hk;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\LoveCelestial", 0,
        KEY_WRITE, &hk) == ERROR_SUCCESS) {
        RegDeleteValueA(hk, "ApiPort");
        RegCloseKey(hk);
    }
    char localApp[MAX_PATH];
    if (GetEnvironmentVariableA("LOCALAPPDATA", localApp, MAX_PATH) > 0) {
        std::string p = std::string(localApp) + "\\LoveCelestial\\port.txt";
        DeleteFileA(p.c_str());
    }
    DeleteFileA("C:\\Users\\Public\\LoveCelestial.port");
}

static mj::JVal CmdStatus() {
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", mj::JVal::boo(true));
    r.set("pid", mj::JVal::num((double)GetCurrentProcessId()));
    r.set("ui_dir", mj::JVal::str(g_dllDir));
    mj::JVal hooks = mj::JVal::obj_();
    auto addHook = [&](const char* name, bool resolved, const char* method, LONG calls) {
        mj::JVal h = mj::JVal::obj_();
        h.set("resolved", mj::JVal::boo(resolved));
        h.set("method", mj::JVal::str(method));
        h.set("calls", mj::JVal::num((double)calls));
        hooks.set(name, std::move(h));
        };
    addHook("blocklist", g_rBlock, g_mBlock, g_cBlock);
    addHook("hash", g_rHash, g_mHash, g_cHash);
    addHook("heuristic", g_rHeur, g_mHeur, g_cHeur);
    r.set("hooks", std::move(hooks));
    return r;
}
static mj::JVal CmdList() {
    mj::JVal r = mj::JVal::obj_();
    mj::JVal resp = TauriInvoke("get_mod_library", mj::JVal::obj_());
    auto* ok = resp.get("ok");
    if (ok && ok->asBool()) {
        r.set("ok", mj::JVal::boo(true));
        auto* val = resp.get("value");
        r.set("library", val ? *val : resp);
    }
    else {
        r.set("ok", mj::JVal::boo(false));
        r.set("raw", resp);
    }
    return r;
}
static mj::JVal CmdImport(const std::string& path) {
    if (path.empty()) {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("missing 'path'"));
        return r;
    }
    DWORD attr = GetFileAttributesA(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("not found: " + path));
        return r;
    }
    std::string lower = path; for (auto& c : lower) c = (char)tolower((unsigned char)c);
    std::string cmd;
    if (attr & FILE_ATTRIBUTE_DIRECTORY)                                      cmd = "import_raw_folder";
    else if (lower.size() > 7 && lower.substr(lower.size() - 7) == ".modpkg")   cmd = "import_modpkg";
    else if (lower.find(".wad") != std::string::npos)                         cmd = "import_wad_client";
    else                                                                      cmd = "import_fantome";

    mj::JVal args = mj::JVal::obj_();
    args.set("path", mj::JVal::str(path));
    args.set("filePath", mj::JVal::str(path));
    args.set("fantomePath", mj::JVal::str(path));
    args.set("modpkgPath", mj::JVal::str(path));
    args.set("wadPath", mj::JVal::str(path));
    args.set("folderPath", mj::JVal::str(path));

    mj::JVal resp = TauriInvoke(cmd, args);
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", resp.get("ok") ? *resp.get("ok") : mj::JVal::boo(true));
    r.set("command", mj::JVal::str(cmd));
    r.set("response", resp);
    return r;
}
static mj::JVal CmdToggle(const std::string& target, bool enable) {
    if (target.empty()) {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("missing 'target'"));
        return r;
    }
    std::string modId = target;
    mj::JVal libResp = TauriInvoke("get_mod_library", mj::JVal::obj_());
    auto* libVal = libResp.get("value");
    if (libVal && libVal->t == mj::JVal::OBJ) {
        auto* mods = libVal->get("mods");
        if (mods && mods->t == mj::JVal::ARR) {
            for (auto& m : mods->arr) {
                auto* id = m.get("id"); auto* nm = m.get("name");
                if (!id) continue;
                if (id->asStr() == target) { modId = target; break; }
                if (nm) {
                    std::string a = nm->asStr(); std::string b = target;
                    for (auto& c : a) c = (char)tolower((unsigned char)c);
                    for (auto& c : b) c = (char)tolower((unsigned char)c);
                    if (a == b) { modId = id->asStr(); break; }
                }
            }
        }
    }
    mj::JVal args = mj::JVal::obj_();
    args.set("modId", mj::JVal::str(modId));
    args.set("enabled", mj::JVal::boo(enable));
    mj::JVal resp = TauriInvoke("toggle_mod", args);
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", resp.get("ok") ? *resp.get("ok") : mj::JVal::boo(true));
    r.set("modId", mj::JVal::str(modId));
    r.set("response", resp);
    return r;
}
static mj::JVal CmdStartPatcher() {
    mj::JVal args = mj::JVal::obj_();
    args.set("config", mj::JVal::obj_());
    mj::JVal resp = TauriInvoke("start_patcher", args);
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", resp.get("ok") ? *resp.get("ok") : mj::JVal::boo(true));
    r.set("response", resp); return r;
}
static mj::JVal CmdStopPatcher() {
    mj::JVal resp = TauriInvoke("stop_patcher", mj::JVal::obj_());
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", resp.get("ok") ? *resp.get("ok") : mj::JVal::boo(true));
    r.set("response", resp); return r;
}
static mj::JVal CmdRawInvoke(const mj::JVal& req) {
    auto* cmd = req.get("command"); auto* args = req.get("args");
    if (!cmd) {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("missing 'command'"));
        return r;
    }
    mj::JVal resp = TauriInvoke(cmd->asStr(), args ? *args : mj::JVal::obj_());
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", resp.get("ok") ? *resp.get("ok") : mj::JVal::boo(true));
    r.set("response", resp); return r;
}
static mj::JVal CmdHookControl(bool enable) {
    MH_STATUS st = enable ? MH_EnableHook(MH_ALL_HOOKS) : MH_DisableHook(MH_ALL_HOOKS);
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", mj::JVal::boo(st == MH_OK));
    r.set("enabled", mj::JVal::boo(enable));
    r.set("status", mj::JVal::num((double)st)); return r;
}
static DWORD WINAPI KillThread(LPVOID) {
    Sleep(200);
    ExitProcess(0);
    return 0;
}
static mj::JVal CmdKill() {
    HANDLE th = CreateThread(NULL, 0, KillThread, NULL, 0, NULL);
    if (th) CloseHandle(th);
    mj::JVal r = mj::JVal::obj_();
    r.set("ok", mj::JVal::boo(true));
    r.set("message", mj::JVal::str("terminating Celestial.exe in 200 ms"));
    return r;
}

static std::string HandleCommand(const std::string& line) {
    mj::JVal req = mj::parse(line);
    if (req.t != mj::JVal::OBJ) {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("invalid JSON"));
        return r.dump();
    }
    auto* cmd = req.get("cmd"); std::string c = cmd ? cmd->asStr() : "";

    if (c == "hello") {
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(true));
        r.set("service", mj::JVal::str("LoveCelestial"));
        r.set("version", mj::JVal::str("1.3"));
        r.set("pid", mj::JVal::num((double)GetCurrentProcessId()));
        r.set("port", mj::JVal::num((double)g_apiPort));
        r.set("ui_html", mj::JVal::str(g_dllDir + "\\ui.html"));
        r.set("credit", mj::JVal::str(kCredit));
        return r.dump();
    }
    if (c == "reload_ui") {
        InterlockedExchange(&g_forceUiReload, 1);
        mj::JVal r = mj::JVal::obj_();
        r.set("ok", mj::JVal::boo(true));
        r.set("message", mj::JVal::str("reloading ui.html"));
        return r.dump();
    }
    if (c == "status")        return CmdStatus().dump();
    if (c == "ping") { mj::JVal r = mj::JVal::obj_(); r.set("ok", mj::JVal::boo(true)); r.set("pong", mj::JVal::num((double)GetTickCount64())); return r.dump(); }
    if (c == "list")          return CmdList().dump();
    if (c == "import")        return CmdImport(req.get("path") ? req.get("path")->asStr() : "").dump();
    if (c == "toggle")        return CmdToggle(req.get("target") ? req.get("target")->asStr() : "",
        req.get("enabled") ? req.get("enabled")->asBool() : true).dump();
    if (c == "start_patcher") return CmdStartPatcher().dump();
    if (c == "stop_patcher")  return CmdStopPatcher().dump();
    if (c == "raw_invoke")    return CmdRawInvoke(req).dump();
    if (c == "hook_enable")   return CmdHookControl(true).dump();
    if (c == "hook_disable")  return CmdHookControl(false).dump();
    if (c == "kill")          return CmdKill().dump();

    mj::JVal r = mj::JVal::obj_();
    r.set("ok", mj::JVal::boo(false)); r.set("error", mj::JVal::str("unknown: " + c));
    return r.dump();
}


static std::string UrlDecode(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            char hex[3] = { s[i + 1], s[i + 2], 0 };
            out += (char)strtol(hex, nullptr, 16);
            i += 2;
        }
        else if (s[i] == '+') {
            out += ' ';
        }
        else {
            out += s[i];
        }
    }
    return out;
}

static void HandleHttpImageCacheRequest(SOCKET c, const std::string& reqHeader) {
    size_t pPos = reqHeader.find("GET /img?p=");
    if (pPos == std::string::npos) {
        const char* nf = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        send(c, nf, (int)strlen(nf), 0);
        return;
    }
    size_t start = pPos + 11;
    size_t end = reqHeader.find_first_of(" &\r\n", start);
    std::string relPath = UrlDecode(reqHeader.substr(start, end - start));

    while (!relPath.empty() && relPath[0] == '/') relPath.erase(0, 1);
    std::string flatName = relPath;
    for (char& ch : flatName) {
        if (ch == '/' || ch == '\\' || ch == ':' || ch == '?' || ch == '*' || ch == '"' || ch == '<' || ch == '>' || ch == '|') {
            ch = '_';
        }
    }

    std::string localFile = GetImgCacheDir() + "\\" + flatName;

    if (GetFileAttributesA(localFile.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::string remoteUrl = "https://raw.communitydragon.org/latest/plugins/rcp-be-lol-game-data/global/default/" + relPath;
        URLDownloadToFileA(NULL, remoteUrl.c_str(), localFile.c_str(), 0, NULL);
    }

    HANDLE h = CreateFileA(localFile.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        const char* nf = "HTTP/1.1 404 Not Found\r\nAccess-Control-Allow-Origin: *\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        send(c, nf, (int)strlen(nf), 0);
        return;
    }

    DWORD sz = GetFileSize(h, NULL);
    std::vector<char> data(sz);
    DWORD rd = 0;
    if (sz > 0) ReadFile(h, data.data(), sz, &rd, NULL);
    CloseHandle(h);

    const char* mime = "application/octet-stream";
    if (flatName.find(".png") != std::string::npos) mime = "image/png";
    else if (flatName.find(".jpg") != std::string::npos || flatName.find(".jpeg") != std::string::npos) mime = "image/jpeg";
    else if (flatName.find(".webp") != std::string::npos) mime = "image/webp";
    else if (flatName.find(".json") != std::string::npos) mime = "application/json";

    char hdr[512];
    int hlen = snprintf(hdr, sizeof(hdr),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Cache-Control: public, max-age=31536000, immutable\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n\r\n",
        mime, (unsigned long)rd);

    send(c, hdr, hlen, 0);
    if (rd > 0) send(c, data.data(), (int)rd, 0);
}

static DWORD WINAPI TcpClientThread(LPVOID param) {
    SOCKET c = (SOCKET)(uintptr_t)param;
    SetSocketTimeout(c, 15000);
    std::string buf;
    char chunk[4096];

    int r = recv(c, chunk, sizeof(chunk), 0);
    if (r <= 0) {
        closesocket(c);
        return 0;
    }
    buf.append(chunk, r);

    if (buf.compare(0, 4, "GET ") == 0) {
        while (buf.find("\r\n\r\n") == std::string::npos && buf.size() < 8192) {
            int r2 = recv(c, chunk, sizeof(chunk), 0);
            if (r2 <= 0) break;
            buf.append(chunk, r2);
        }
        HandleHttpImageCacheRequest(c, buf);
        shutdown(c, SD_BOTH);
        closesocket(c);
        return 0;
    }

    while (true) {
        try {
            if (buf.size() > 1024 * 1024) break;
            size_t nl;
            while ((nl = buf.find('\n')) != std::string::npos) {
                std::string line = buf.substr(0, nl);
                buf.erase(0, nl + 1);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                if (line.empty()) continue;

                std::string resp = HandleCommand(line) + "\n";
                if (send(c, resp.data(), (int)resp.size(), 0) <= 0) goto done;
            }
        }
        catch (...) { break; }

        r = recv(c, chunk, sizeof(chunk), 0);
        if (r <= 0) break;
        buf.append(chunk, r);
    }
done:
    shutdown(c, SD_BOTH);
    closesocket(c);
    return 0;
}

static SOCKET TryBindPort(int port) {
    SOCKET ls = socket(AF_INET, SOCK_STREAM, 0);
    if (ls == INVALID_SOCKET) return INVALID_SOCKET;
    BOOL exclusive = TRUE;
    setsockopt(ls, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&exclusive, sizeof(exclusive));
    sockaddr_in a{}; a.sin_family = AF_INET; a.sin_port = htons((u_short)port);
    inet_pton(AF_INET, "127.0.0.1", &a.sin_addr);
    if (bind(ls, (sockaddr*)&a, sizeof(a)) != 0) { closesocket(ls); return INVALID_SOCKET; }
    if (listen(ls, SOMAXCONN) != 0) { closesocket(ls); return INVALID_SOCKET; }
    return ls;
}

static DWORD WINAPI TcpServerThread(LPVOID) {
    int rangeStart = kApiPortStart, rangeEnd = kApiPortEnd;
    char envPort[16] = {};
    if (GetEnvironmentVariableA("CELESTIAL_API_PORT", envPort, 16) > 0) {
        int p = atoi(envPort);
        if (p > 0 && p < 65536) { rangeStart = rangeEnd = p; }
    }
    SOCKET ls = INVALID_SOCKET;
    for (int p = rangeStart; p <= rangeEnd; ++p) {
        ls = TryBindPort(p);
        if (ls != INVALID_SOCKET) { g_apiPort = p; break; }
    }
    if (ls == INVALID_SOCKET) return 0;

    BroadcastPort(g_apiPort);

    while (WaitForSingleObject(g_hStopEvt, 0) != WAIT_OBJECT_0) {
        fd_set rs; FD_ZERO(&rs); FD_SET(ls, &rs);
        timeval tv{ 0, 200000 };
        int r = select(0, &rs, NULL, NULL, &tv);
        if (r <= 0) continue;
        SOCKET c = accept(ls, NULL, NULL);
        if (c == INVALID_SOCKET) continue;
        HANDLE th = CreateThread(NULL, 0, TcpClientThread, (LPVOID)(uintptr_t)c, 0, NULL);
        if (th) CloseHandle(th);
        else closesocket(c);
    }
    closesocket(ls);
    ClearPortBroadcast();
    g_apiPort = 0;
    return 0;
}

static void InstallHooks() {
    HMODULE target = GetModuleHandleA("Celestial.exe");
    if (!target) return;
    uintptr_t base = (uintptr_t)target;

    struct H { uintptr_t addr; bool ok; };
    H hb{ base + 0x4F11E0,  VerifyPrologue(base + 0x4F11E0, PRO_BLOCK, 16) };
    H hh{ base + 0x4E70F0,  VerifyPrologue(base + 0x4E70F0, PRO_HASH,  16) };
    H hr{ base + 0x161FD90, VerifyPrologue(base + 0x161FD90, PRO_HEUR, 16) };

    if (hb.ok && MH_CreateHook((LPVOID)hb.addr, (LPVOID)&HookBlock, (LPVOID*)&g_origBlock) == MH_OK) { g_rBlock = true; g_mBlock = "rva"; }
    if (hh.ok && MH_CreateHook((LPVOID)hh.addr, (LPVOID)&HookHash, (LPVOID*)&g_origHash) == MH_OK) { g_rHash = true; g_mHash = "rva"; }
    if (hr.ok && MH_CreateHook((LPVOID)hr.addr, (LPVOID)&HookHeur, (LPVOID*)&g_origHeur) == MH_OK) { g_rHeur = true; g_mHeur = "rva"; }

    MH_EnableHook(MH_ALL_HOOKS);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        if (InterlockedCompareExchange(&g_dllAttached, 1, 0) != 0) return TRUE;
        DisableThreadLibraryCalls(hModule);
        LogInit(kHookLog); LogInit(kApiLog);

        char dllPath[MAX_PATH] = {};
        if (GetModuleFileNameA(hModule, dllPath, MAX_PATH) > 0) {
            std::string p(dllPath);
            size_t slash = p.find_last_of("\\/");
            if (slash != std::string::npos) g_dllDir = p.substr(0, slash);
        }

        InitializeCriticalSection(&g_cdpLock);

        wchar_t skip[8] = {};
        if (GetEnvironmentVariableW(L"CELESTIAL_NO_CDP", skip, 8) == 0) {
            if (GetEnvironmentVariableW(L"WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS", NULL, 0) == 0) {
                SetEnvironmentVariableW(L"WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS",
                    L"--remote-debugging-port=9222 --remote-allow-origins=*");
            }
        }

        WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);

        if (MH_Initialize() != MH_OK) return FALSE;
        InstallHooks();

        g_hStopEvt = CreateEventA(NULL, TRUE, FALSE, NULL);
        HANDLE t = CreateThread(NULL, 0, TcpServerThread, NULL, 0, NULL);
        if (t) CloseHandle(t);

        HANDLE tUi = CreateThread(NULL, 0, CustomUiThread, NULL, 0, NULL);
        if (tUi) CloseHandle(tUi);

        wchar_t hideEnv[8] = {};
        if (GetEnvironmentVariableW(L"CELESTIAL_HIDE", hideEnv, 8) > 0) {
            HANDLE th = CreateThread(NULL, 0, HideThread, NULL, 0, NULL);
            if (th) CloseHandle(th);
        }
    }
    else if (reason == DLL_PROCESS_DETACH) {
        if (g_hStopEvt) SetEvent(g_hStopEvt);
        Sleep(400);
        g_wsCache.close();
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        WSACleanup();
        if (g_hStopEvt) { CloseHandle(g_hStopEvt); g_hStopEvt = nullptr; }
        DeleteCriticalSection(&g_cdpLock);
    }
    return TRUE;
}
