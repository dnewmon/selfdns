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

#include "base.h"
#include "socket.h"

Socket *Socket::TcpSocket()
{
	return new Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

Socket *Socket::UdpSocket()
{
	return new Socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

int Socket::AddToSet(Socket* socket, fd_set *set, int lastMax)
{
	FD_SET(socket->m_socketfd, set);
	
	return (socket->m_socketfd > lastMax ? (socket->m_socketfd + 1) : lastMax);
}

bool Socket::IsSet(Socket* socket, fd_set *set)
{
	return FD_ISSET(socket->m_socketfd, set);
}


int Socket::Select(Socket * const *sockets, 
			    Socket **readable, Socket **writeable, Socket **errored, 
			    size_t socketCount, timeval *timeout)
{
	int maxfd = 0;
	int i;
	
	fd_set read_set, write_set, error_set;
	
	bzero(readable, sizeof(Socket *) * (socketCount + 1));
	bzero(writeable, sizeof(Socket *) * (socketCount + 1));
	bzero(errored, sizeof(Socket *) * (socketCount + 1));
	
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&error_set);
	
	for (i = 0; i < socketCount; i++) {
		const Socket * socket = sockets[i];
		
		FD_SET(socket->m_socketfd, &read_set);
		FD_SET(socket->m_socketfd, &write_set);
		FD_SET(socket->m_socketfd, &error_set);
		
		maxfd = (socket->m_socketfd > maxfd) ? socket->m_socketfd : maxfd;
	}
	
	maxfd++;
	
	int ready = select(maxfd, &read_set, &write_set, &error_set, timeout);
	
	int readIndex = 0;
	int writeIndex = 0;
	int errorIndex = 0;
	
	for (i = 0; i < socketCount; i++) {
		if (FD_ISSET(sockets[i]->m_socketfd, &read_set)) {
			readable[readIndex++] = sockets[i];
		}
		
		if (FD_ISSET(sockets[i]->m_socketfd, &write_set)) {
			writeable[writeIndex++] = sockets[i];
		}
		
		if (FD_ISSET(sockets[i]->m_socketfd, &error_set)) {
			errored[errorIndex++] = sockets[i];
		}
	}
	
	return ready;
}

Socket::Socket(int domain, int type, int protocol)
{
	this->m_socketfd = socket(domain, type, protocol);
}

Socket::~Socket()
{
	close(this->m_socketfd);
}

int Socket::GetFlags()
{
	return fcntl(this->m_socketfd, F_GETFL, 0);
}

int Socket::SetFlags(int flags)
{
	return fcntl(this->m_socketfd, F_SETFL, flags);
}

int Socket::Connect(const sockaddr* serv_addr, socklen_t addrlen)
{
	return connect(this->m_socketfd, serv_addr, addrlen);
}

int Socket::Accept(sockaddr* addr, socklen_t* addrlen)
{
	return accept(this->m_socketfd, addr, addrlen);
}

int Socket::Bind(sockaddr* my_addr, socklen_t addrlen)
{
	int yes=1;

	// lose the pesky "Address already in use" error message
	if (setsockopt(this->m_socketfd, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	return bind(this->m_socketfd, my_addr, addrlen);
}

int Socket::Listen(int backlog)
{
	return listen(this->m_socketfd, backlog);
}

int Socket::Recieve(void* msg, size_t len, int flags)
{
	return recv(this->m_socketfd, msg, len, flags);
}

int Socket::Send(const void* msg, size_t len, int flags)
{
	return send(this->m_socketfd, msg, len, flags);
}

int Socket::SendTo(const void* msg, size_t len, int flags, const sockaddr* to, socklen_t tolen)
{
	return sendto(this->m_socketfd, msg, len, flags, to, tolen);
}

int Socket::RecieveFrom(void* buf, size_t len, int flags, sockaddr* from, socklen_t* fromlen)
{
	return recvfrom(this->m_socketfd, buf, len, flags, from, fromlen);
}

bool Socket::operator==(const Socket& socket)
{
	return this->m_socketfd == socket.m_socketfd;
}
