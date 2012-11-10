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
		
		sockaddr * clientAddr = (sockaddr *)malloc(256);
		socklen_t clientAddrLen = 256;
		unsigned char szBuffer[1024];
		
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
					clientAddrLen = 256;
					
					int length = server->RecieveFrom(&szBuffer[0], 1024, 0, clientAddr, &clientAddrLen);
					
					DnsPacket * packet = DnsParsing::decodePacket(&szBuffer[0]);
					DnsPacket * response = new DnsPacket();
					memcpy(response, packet, sizeof(DnsHeader));
					
					response->flags = 0;
					response->qr = 1;
					response->recurse = 1;
					response->recursiveAvailable = 1;
					
					response->questionCount = 1;
					response->answerCount = 1;
					
					response->answers = new DnsResource[1];
					response->questions = new DnsResource[1];
					
					DnsResource * ans = response->answers;
					
					ans->name = packet->questions[0].name;
					ans->nClass = 1;
					ans->nType = 1;
					ans->ttl = 1;
					ans->length = 4;
					ans->payload = new unsigned char[4];
					ans->payload[0] = 127;
					ans->payload[1] = 0;
					ans->payload[2] = 0;
					ans->payload[3] = 1;
					
					ans = response->questions;
					ans->name = packet->questions[0].name;
					ans->nClass = 1;
					ans->nType = 1;
					
					unsigned char * entirePacket = DnsParsing::encodePacket(response, &length);
					server->SendTo(entirePacket, length, 0, clientAddr, clientAddrLen);
					
					delete [] entirePacket;
					
					DnsParsing::releasePacket(packet);
					
					response->answers->name = 0;
					response->questions->name = 0;
					DnsParsing::releasePacket(response);
					
				} else if (Socket::IsSet(server, &write_set)) {
					cout << "Writable" << endl;
				} else if (Socket::IsSet(server, &error_set)) {
					cout << "Errored" << endl;
				}
			}
		}
		
		free(clientAddr);
	} else {
		cout << "Unable to bind!" << endl;
	}
	
	delete server;
	
	cout << "Done" << endl;
	return 0;
}
