/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SOCKET_H
#define SOCKET_H

class Socket
{
private:
	int m_socketfd;
	
public:
	Socket(int domain, int type, int protocol);
	virtual ~Socket();
	
	static Socket *TcpSocket();
	static Socket *UdpSocket();
	
	static int AddToSet(Socket* socket, fd_set *set, int lastMax);
	static bool IsSet(Socket* socket, fd_set *set);
	
	static int Select(Socket * const *sockets, 
					Socket **readable, Socket **writeable, Socket **errored, 
					size_t socketCount, timeval *timeout);
	
	int SetFlags(int flags);
	int GetFlags();
	
	int Connect(const sockaddr *serv_addr, socklen_t addrlen);
	int Listen(int backlog);
	int Accept(sockaddr *addr, socklen_t *addrlen);
	int Bind(sockaddr *my_addr, socklen_t addrlen);
	int Send(const void *msg, size_t len, int flags);
	int SendTo(const void *msg, size_t len, int flags, const sockaddr *to, socklen_t tolen);
	int Recieve(void *msg, size_t len, int flags);
	int RecieveFrom(void *buf, size_t len, int flags, sockaddr *from, socklen_t *fromlen);
	
	bool operator == (const Socket & socket);
};

#endif // SOCKET_H

struct stat;
