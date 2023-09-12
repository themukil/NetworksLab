#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>

using namespace std;

#define MAX 100
#define port 3002

mutex inputMutex;
mutex outputMutex;

void receiveAndPrint(int clientSocket) {
    while (true) {
        char receiveMessage[MAX];
        int rMsgSize = recv(clientSocket, receiveMessage, MAX, 0);
        if (rMsgSize <= 0) {
            cout << "Server disconnected." << endl;
            break;
        }
        lock_guard<mutex> lock(outputMutex);
        cout << "Received: " << receiveMessage << endl;
    }
}

void getInputAndSend(int clientSocket) {
    while (true) {
        string s;
        char input[MAX];
        getline(cin, s);
        lock_guard<mutex> lock(inputMutex);

        int n = s.size();
        for (int i = 0; i < n; i++) {
            input[i] = s[i];
        }
        input[n] = '\0';
        send(clientSocket, input, strlen(input) + 1, 0);
    }
}

int main() {
    int clientSocket, serverSocket, receiveMsgSize;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);
    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    thread receivePrintThread(receiveAndPrint, clientSocket);
    thread getInputThread(getInputAndSend, clientSocket);
    receivePrintThread.join();
    getInputThread.join();
    close(clientSocket);
    return 0;
}
