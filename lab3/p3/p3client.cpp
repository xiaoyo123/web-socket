#include <stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <string>
#include <iostream>
#include <thread>
using namespace std;

const wchar_t* GetWC(const char* c) {
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    size_t convertedChar = 0;
    mbstowcs_s(&convertedChar, wc, cSize, c, _TRUNCATE);
    return wc;
}
void getMes(SOCKET sock) {
    while (true) {
        char szBuffer[20000] = { 0 };
        recv(sock, szBuffer, 20000, NULL);
        cout << szBuffer << endl;
    }
}

int main() {
    WSAData wsaData;

    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }

    SOCKET sock;

    sockaddr_in addr;
    InetPton(AF_INET, GetWC("127.0.0.1"), &addr.sin_addr.s_addr); // 設定 IP
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); // 設定 port,htons()跟網路位元組順序有關

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    connect(sock, (SOCKADDR*)&addr, sizeof(SOCKADDR));

    char szBuffer[20000] = { 0 };
    thread get_mes(getMes, sock);
    get_mes.detach();

    while (true) {

        string mes;
        getline(cin, mes);
        char* sendbuf = const_cast<char*>(mes.c_str());
        send(sock, sendbuf, (int)strlen(sendbuf), 0);

    }

    closesocket(sock);

    WSACleanup();

    return 0;
}