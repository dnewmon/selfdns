#include "base.h"
#include "socket.h"
#include "list.h"

int main(int argc, char **argv) {
	Socket *server = Socket::UdpSocket();
	
	sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(53000);
	
	List<Socket *> sockets;
	sockets.Add(server);
	
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	
	if (server->Bind((sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
		
		while (true) {
			Socket **readable = new Socket*[sockets.Count() + 1];
			Socket **writable = new Socket*[sockets.Count() + 1];
			Socket **errored = new Socket*[sockets.Count() + 1];
			
			int ready = Socket::Select(sockets.ptr(), readable, writable, errored, sockets.Count(), &timeout);
			
			if (ready != 0) {
				Socket * cur = *readable;
				
				while (cur != 0) {
					if (cur == server) {
						cout << "Recieve" << endl;
					}
					
					cur++;
				}
			}
			
			delete [] readable;
			delete [] writable;
			delete [] errored;
		}
		
	} else {
		cout << "Unable to bind!" << endl;
	}
	
	delete server;
	
	cout << "Done" << endl;
	return 0;
}
