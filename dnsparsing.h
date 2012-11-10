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


#ifndef DNSPARSING_H
#define DNSPARSING_H

#pragma pack(push)
#pragma pack(1)

struct DnsHeader;
struct DnsPacket;
struct DnsResourceRecord;
struct DnsResource;

struct DnsHeader
{
	unsigned short id;
	union {
		unsigned short flags;
		struct {
			unsigned char responseCode: 4;
			unsigned char reserved : 3;
			unsigned char recursiveAvailable : 1;
			
			unsigned char recurse : 1;
			unsigned char truncated : 1;
			unsigned char authoritative : 1;
			unsigned char opcode : 4;
			unsigned char qr : 1;
		};
	};
	
	short questionCount;
	short answerCount;
	unsigned short authorityCount;
	unsigned short additionalCount;
};

struct DnsPacket : public DnsHeader
{
	DnsResource * answers;
	DnsResource * questions;
	
	unsigned char * firstByte;
};

struct DnsQuestion
{
	unsigned short nType;
	unsigned short nClass;
};

struct DnsResourceRecord : public DnsQuestion
{
	unsigned int ttl;
	unsigned short length;
};

struct DnsResource : public DnsResourceRecord
{
	char *name;
	unsigned char * payload;
};

class DnsParsing
{
public:
	static DnsPacket * decodePacket(unsigned char * buffer);
	static void decodeHeader(DnsHeader * header);
	static unsigned char * decodeResource(DnsPacket * packet, DnsResource * resource, unsigned char * buffer, bool question);
	static char * decodeName(DnsPacket * packet, unsigned char ** buffer);
	
	static char * encodeName(char * name, int * length);
	static void encodeResource(DnsResource * resource);
	static void encodeHeader(DnsHeader * header);
	static unsigned char * encodePacket(DnsPacket* packet, int * length);
	
	static void releasePacket(DnsPacket * packet);
};


#pragma pack(pop)

#endif // DNSPARSING_H
