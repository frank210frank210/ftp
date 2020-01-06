#pragma comment(lib, "Ws2_32.lib")

#include <winsock2.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string confirm;
    char message[200];
    char cmd[200];
    int result;

    //�}�l Winsock-DLL
    int r;
    WSAData wsaData;
    WORD DLLVersion;
    DLLVersion = MAKEWORD(2,1);
    r = WSAStartup(DLLVersion, &wsaData);

    //�ŧi�� socket �ϥΪ� sockadder_in ���c
    SOCKADDR_IN addr;

    int addlen = sizeof(addr);

    //�]�w socket
    SOCKET sConnect;

    //AF_INET: internet-family
    //SOCKET_STREAM: connection-oriented socket
    sConnect = socket(AF_INET, SOCK_STREAM, NULL);

    //�]�w addr ���
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);

    cout << "connect to server?[Y] or [N]" << endl;
    cin >> confirm;

    if(confirm == "N")
    {
        exit(1);
    }else{
        if(confirm == "Y")
        {

            result = connect(sConnect, (SOCKADDR*)&addr, sizeof(addr));

            if(result == -1){
                cout << "connect error" << endl;
                closesocket(sConnect);
                exit(0);
            }

            //���� server �ݪ��T��
            ZeroMemory(message, 200);
            recv(sConnect, message, sizeof(message), 0);
            cout << message << endl;

            while(1){
                ZeroMemory(cmd, 200);
                ZeroMemory(message, 200);
                cin >> cmd;
                if(strcmp(cmd, "exit") == 0){
                    break;
                }

                send(sConnect, cmd, sizeof(cmd), 0);
                recv(sConnect, message, sizeof(message), 0);
                cout << message << endl;
            }

            closesocket(sConnect);
            cin >> cmd;
        }
    }
}
