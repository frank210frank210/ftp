#pragma comment(lib, "Ws2_32.lib")
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

char* getfilelist(){
    DIR    *dir;
    struct    dirent    *ptr;
    struct stat s_buf;
    dir = opendir("."); ///open the dir
    char *res = new char[1000];
    memset(res,'\0',sizeof(1000));
    strcat(res,"filelist:");

    while((ptr = readdir(dir)) != NULL) ///read the list of this dir
    {
        char file_path[200];
        memset(file_path,0,200);
        strcat(file_path,".");
        strcat(file_path,"\\");
        strcat(file_path,ptr->d_name);

        stat(file_path,&s_buf);

        S_ISDIR(s_buf.st_mode);
        if(S_ISREG(s_buf.st_mode))
        {
            strcat(res," ");
            strcat(res,ptr->d_name);
        }
    }
    cout << res << endl;
    closedir(dir);
    return res;
}

int main()
{
    int r;
    int result;
    char cmd[200];
    char msg[200];
    WSAData wsaData;
    WORD DLLVSERION;
    DLLVSERION = MAKEWORD(2,1);//Winsocket-DLL 版本

    //用 WSAStartup 開始 Winsocket-DLL
    r = WSAStartup(DLLVSERION, &wsaData);

    //宣告 socket 位址資訊(不同的通訊,有不同的位址資訊,所以會有不同的資料結構存放這些位址資訊)
    SOCKADDR_IN addr;
    int addrlen = sizeof(addr);

    //建立 socket
    SOCKET sListen;  //listen用
    SOCKET sconnect;  //read write用

    //設定位址資訊的資料
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);

    //設定 Listen
    sListen = socket(AF_INET, SOCK_STREAM, NULL);
    if(bind(sListen,(SOCKADDR*)&addr, sizeof(addr)) == -1){
        cout << "Bind error!\n";
        close(sListen);
        exit(-1);
    }
    cout << "Bind successfully.\n";
    listen(sListen, SOMAXCONN);//SOMAXCONN: listening without any limit

    //設定select
    struct timeval tv = {3,500000};
    fd_set readfds;
    fd_set writefds;
    fd_set connfds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&connfds);
    FD_SET(sListen, &connfds);

    //等待連線
    SOCKADDR_IN clinetAddr;

    int maxfd = sListen;

    while(true)
    {
        readfds = connfds;

        result = select(maxfd+1, &readfds, NULL, NULL, &tv);
        if(result == -1){
            cout <<"connect select() error /n";
            break;
        }else if(result == 0){
            cout << "FD " << sListen <<" waiting connect..." << endl;
        }
        else{
            for(int i = 0; i<=maxfd; i++){
                if(FD_ISSET(i,&readfds)){
                    //new connection
                    if(i==sListen){
                        sconnect = accept(sListen, (SOCKADDR*)&clinetAddr, &addrlen);
                        cout << "client connect FDid " << sconnect << endl;
                        if(maxfd < sconnect){
                            maxfd = sconnect;
                        }
                        FD_SET(sconnect, &connfds);

                        string s;
                        stringstream ss(s);
                        ss << sconnect;
                        string buf = "connect successful Your FDid is " + ss.str();
                        const char *sendbuf = buf.c_str();
                        send(sconnect, sendbuf, (int)strlen(sendbuf), 0);
                        cout << "fd "  << sconnect << " connect" << endl;
                    }
                    //read ready
                    else{
                        ZeroMemory(cmd, 200);
                        ZeroMemory(msg, 200);
                        result = recv(i,cmd,sizeof(cmd),0);
                        if(result == -1){
                            cout << "error recv fd " << i << endl;
                            close(i);
                            FD_CLR(i, &connfds);
                        }else if(result == 0){
                            cout << "client disconnect fd " << i << endl;
                            close(i);
                            FD_CLR(i, &connfds);
                        }else{
                            cout << "recv:" << cmd << "    read from fd " << i << endl;

                            if(strcmp(cmd, "list") == 0){
                                char *tmp;
                                tmp = getfilelist();
                                send(i, tmp, (int)strlen(tmp), 0);
                            }else if(strcmp(cmd, "get") == 0){
                            }else if(strcmp(cmd, "put") == 0){
                            }else{
                                char* tmsg = "undefine cmd";
                                cout << cmd << " undefine cmd" << endl;
                                send(i, tmsg, (int)strlen(tmsg), 0);
                            }
                        }
                    }
                }
            }
        }
    }
    closesocket(sListen);
}



/*
            if(FD_ISSET(sListen, &connfds)) {
                sconnect = accept(sListen, (SOCKADDR*)&clinetAddr, &addrlen);
                cout << "client connect FDid " << sconnect << endl;
                if(maxfd < sconnect){
                    maxfd = sconnect;
                }
                FD_SET(sconnect, &readfds);
                FD_SET(sconnect, &writefds);

                char *sendbuf = "server sending data test";
                send(sconnect, sendbuf, (int)strlen(sendbuf), 0);
                cout << "write fd "  << sconnect << endl;

                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                FD_SET(sconnect, &readfds);
                FD_SET(sconnect, &writefds);
                if(SOCKET_ERROR!=select(maxfd+1,&readfds,&writefds,0,0)){
                    if(FD_ISSET(sconnect,&readfds)){
                        char cmd[200];
                        recv(sconnect,cmd,sizeof(cmd),0);
                        cout << cmd << endl;
                        cout << "read fd " << sconnect << endl;
                    }
                    if(FD_ISSET(sconnect,&writefds)){
                        char *sendbuf = "server sending data test";
                        send(sconnect, sendbuf, (int)strlen(sendbuf), 0);
                        cout << "write fd "  << sconnect << endl;
                    }
                }

            }
            */



