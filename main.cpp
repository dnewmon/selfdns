#include "base.h"
#include "socket.h"

int main(int argc, char **argv) {
	Socket *server = Socket::UdpSocket();
	
	sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(53000);
	
	if (server->Bind((sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
		
	} else {
		cout << "Unable to bind!" << endl;
	}
	
	delete server;
	return 0;
}
