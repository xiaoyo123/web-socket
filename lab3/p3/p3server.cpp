#include <winsock2.h>
#include <stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <cstdlib>
#include <thread>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

vector<pair<string, SOCKET> > ip;

const wchar_t* GetWC(const char* c) {
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    size_t convertedChar = 0;
    mbstowcs_s(&convertedChar, wc, cSize, c, _TRUNCATE);
    return wc;
}

void check(SOCKET Connect, int idx) {
    while (true) {
        char recvbuf[20000] = { '\0' };
        int now = recv(Connect, recvbuf, sizeof(recvbuf), 0);
        if (now == 0 || now == -1) break;

        string mes = ip[idx].first + ": " + recvbuf;
        cout << mes << endl;

        for (int i = 0; i < ip.size(); i++)
            send(ip[i].second, mes.c_str(), (int)mes.size(), 0);
        memset(recvbuf, 0, sizeof(recvbuf));
    }
}
//*
int main() {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    SOCKET sListen;
    SOCKET sConnect;
    sockaddr_in addr;
    InetPton(AF_INET, GetWC("127.0.0.1"), &addr.sin_addr.s_addr); // 設定 IP
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); // 設定 port,htons()跟網路位元組順序有關

    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //AF_INET:IPv4    AF_INET6:IPv6
    //SOCK_STREAM:TCP  SOCK_DGRAM:UDP
    //IPPROTO_TCP:TCP  IPIROTO_UDP:UDP
    bind(sListen, (SOCKADDR*)&addr, sizeof(SOCKADDR)); //server端綁定位置
    listen(sListen, 20);


    vector<thread> clients;

    while (true) {
        sockaddr_in clientAddr; // client 端位址資訊
        int clientAddrLen = sizeof(clientAddr);
        sConnect = accept(sListen, (SOCKADDR*)&clientAddr, &clientAddrLen);

        if (sConnect != INVALID_SOCKET)
        {
            // 有 client 端成功連線過來
            char clientIP[20];
            inet_ntop(AF_INET, (void*)&clientAddr, clientIP, 20);
            printf("server: got connection from %s\n", clientIP);
            ip.emplace_back(clientIP, sConnect);

            thread newConnect(check, sConnect, ip.size() - 1);
            newConnect.detach();
        }
    }

    closesocket(sConnect);
    closesocket(sListen);

    WSACleanup();

    return 0;
}//*/