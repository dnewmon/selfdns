#include "base.h"
#include "socket.h"
#include "dnsparsing.h"
#include "list.h"

int main(int argc, char **argv) {
	Socket *server = Socket::UdpSocket();
	
	server->SetFlags(server->GetFlags()|O_NONBLOCK);
	
	sockaddr_in serverAddr;
	bzero(&serverAddr, sizeof(serverAddr));
	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(53);
	
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	
	if (server->Bind((sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
		cout << "Ready!" << endl;
		
		sockaddr clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		unsigned char szBuffer[256];
		
		fd_set read_set, write_set, error_set;
		
		while (true) {
			FD_ZERO(&read_set);
			FD_ZERO(&write_set);
			FD_ZERO(&error_set);
			
			int max = 0;
			
			max = Socket::AddToSet(server, &read_set, max);
			//max = Socket::AddToSet(server, &write_set, max);
			max = Socket::AddToSet(server, &error_set, max);
			
			int ready = select(max, &read_set, &write_set, &error_set, &timeout);
			
			if (ready != 0) {
				if (Socket::IsSet(server, &read_set)) {
					int length = server->RecieveFrom(&szBuffer[0], 256, 0, &clientAddr, &clientAddrLen);
					
					DnsPacket * packet = DnsParsing::decodePacket(&szBuffer[0]);
					
					cout << "Answers: " << packet->answerCount << endl;
					cout << "Questions: " << packet->questionCount << endl;
					cout << "Resources: " << packet->resourceCount << endl;
					cout << "Additional: " << packet->additionalCount << endl;
					
					DnsParsing::releasePacket(packet);
					
					cout << "Recieved: " << length << endl;
				} else if (Socket::IsSet(server, &write_set)) {
					cout << "Writable" << endl;
				} else if (Socket::IsSet(server, &error_set)) {
					cout << "Errored" << endl;
				}
			}
		}
		
	} else {
		cout << "Unable to bind!" << endl;
	}
	
	delete server;
	
	cout << "Done" << endl;
	return 0;
}
