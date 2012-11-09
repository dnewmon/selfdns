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

struct DnsHeader;
struct DnsPacket;
struct DnsResourceRecord;
struct DnsResource;

struct DnsHeader
{
	unsigned short id;
	unsigned short flags;
	unsigned short questionCount;
	unsigned short answerCount;
	unsigned short resourceCount;
	unsigned short additionalCount;
};

struct DnsPacket : public DnsHeader
{
	DnsResource * answers;
	DnsResource * questions;
	
	unsigned char * firstByte;
};

struct DnsResourceRecord
{
	unsigned short nType;
	unsigned short nClass;
	unsigned short ttl;
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
	static unsigned char * decodeResource(DnsPacket * packet, DnsResource * resource, unsigned char * buffer);
	static char * decodeName(DnsPacket * packet, unsigned char ** buffer);
	
	static void releasePacket(DnsPacket * packet);
};

#endif // DNSPARSING_H

class stat;
