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
					clientAddrLen = 256;
					
					int length = server->RecieveFrom(&szBuffer[0], 256, 0, clientAddr, &clientAddrLen);
					
					for (int z = 0; z < length; z++) {
						cout << hex << (int)szBuffer[z] << " ";
						if (z % 4 == 1 && z > 0) {
							cout << endl;
						}
					}
					
					cout << endl << endl;
					
					DnsPacket * packet = DnsParsing::decodePacket(&szBuffer[0]);
					
					cout << "Answers: " << packet->answerCount << endl;
					cout << "Questions: " << packet->questionCount << endl;
					cout << "Authorities: " << packet->authorityCount << endl;
					cout << "Additional: " << packet->additionalCount << endl;
					cout << endl;
					
					DnsPacket * response = new DnsPacket();
					memcpy(response, packet, sizeof(DnsHeader));
					
					response->flags = ntohs(0x80);
					
					response->questionCount = 0;
					response->answerCount = 1;
					
					response->answers = new DnsResource[1];
					response->questions = 0;
					
					DnsResource * ans = response->answers;
					
					ans->name = packet->questions[0].name;
					ans->nClass = 1;
					ans->nType = 1;
					ans->ttl = 0;
					ans->length = 4;
					ans->payload = new unsigned char[4];
					ans->payload[0] = 1;
					ans->payload[1] = 0;
					ans->payload[2] = 0;
					ans->payload[3] = 127;
					
					unsigned char * entirePacket = DnsParsing::encodePacket(response, &length);
					server->SendTo(entirePacket, length, 0, clientAddr, clientAddrLen);
					
					
					for (int z = 0; z < length; z++) {
						cout << hex << (int)entirePacket[z] << " ";
						if (z % 4 == 1 && z > 0) {
							cout << endl;
						}
					}
					
					cout << endl;
					
					delete [] entirePacket;
					
					DnsParsing::releasePacket(packet);
					
					ans->name = 0;
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
