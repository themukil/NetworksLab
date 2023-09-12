
#include<bits/stdc++.h>
using namespace std;
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h>
#define MAX 100
#define port 3002

int main()
{
	int serverSocketHandler = socket(AF_INET , SOCK_STREAM , 0);
	struct sockaddr_in serverAddr , clientAddr,cli;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	bind(serverSocketHandler , (struct sockaddr*) & serverAddr , sizeof(serverAddr));
	listen(serverSocketHandler , 5);
	
	char buff[MAX];
	int clientSocketHandler;
	socklen_t len = sizeof(clientAddr);
	socklen_t lm = sizeof(cli);
	int connection1,con2;
	connection1 = accept(serverSocketHandler , (struct sockaddr*) & clientAddr , &len);
	con2 = accept(serverSocketHandler,(sockaddr*)&cli,&lm);
    string expression;
    
	while(true)
        {
            recv(connection1 , buff , MAX , 0);
			cout<<buff<<endl;
			send(con2 , buff , 100, 0);
			recv(con2,buff,100,0);
			cout<<buff<<endl;
			send(connection1,buff,100,0);
		}
	close(serverSocketHandler);
	return 0;

}
