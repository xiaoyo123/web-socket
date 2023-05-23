#include <stdio.h>
#include <winsock2.h>
#include <Ws2tcpip.h>

const wchar_t* GetWC(const char *c){
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    size_t convertedChar = 0;
    mbstowcs_s(&convertedChar, wc, cSize, c, _TRUNCATE);
    return wc;
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

    connect(sock, (SOCKADDR*)& addr, sizeof(SOCKADDR));
    
    char szBuffer[MAXBYTE] = { 0 };
    
    int guess;
    int iResult;
    while (true) {
        printf("Your guess(1-9): ");
        scanf_s("%d", &guess);

        char sendbuf[2];
        sprintf_s(sendbuf, sizeof(sendbuf), "%d", guess);

        iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
        }
        else if (iResult > 0) {
            recv(sock, szBuffer, MAXBYTE, NULL);
            printf("Data received: %s\n", szBuffer);
            if (strcmp(szBuffer, "Bingo!") == 0)
                break;
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            break;
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(sock);
            WSACleanup();
            return 1;
        }
    }

    closesocket(sock);

    WSACleanup();

    return 0;
}