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
#include "dnsparsing.h"


DnsPacket * DnsParsing::decodePacket(unsigned char * buffer)
{
	DnsPacket * packet = new DnsPacket();
	DnsHeader * header = packet;
	
	memcpy(header, buffer, sizeof(DnsHeader));
	buffer += sizeof(DnsHeader);
	
	packet->firstByte = buffer;
	
	DnsParsing::decodeHeader(header);
	
	packet->answers = new DnsResource[packet->answerCount];
	packet->questions = new DnsResource[packet->questionCount];
	
	int i;
	
	for (i = 0; i < packet->answerCount; i++) {
		buffer = DnsParsing::decodeResource(packet, &packet->answers[i], buffer);
	}
	for (i = 0; i < packet->questionCount; i++) {
		buffer = DnsParsing::decodeResource(packet, &packet->questions[i], buffer);
	}
	
	return packet;
}

void DnsParsing::decodeHeader(DnsHeader * header)
{
	header->id = ntohs(header->id);
	header->flags = ntohs(header->flags);
	header->answerCount = ntohs(header->answerCount);
	header->questionCount = ntohs(header->questionCount);
	header->authorityCount = ntohs(header->authorityCount);
	header->additionalCount = ntohs(header->additionalCount);
}

unsigned char * DnsParsing::decodeResource(DnsPacket * packet, DnsResource * resource, unsigned char * buffer)
{
	resource->name = DnsParsing::decodeName(packet, &buffer);
	
	DnsResourceRecord * record = resource;
	memcpy(record, buffer, sizeof(DnsResourceRecord));
	
	buffer += sizeof(DnsResourceRecord);
	
	record->nType = ntohs(record->nType);
	record->nClass = ntohs(record->nClass);
	record->ttl = ntohs(record->ttl);
	record->length = ntohs(record->length);
	
	resource->payload = buffer;
	
	return buffer + resource->length;
}

char * DnsParsing::decodeName(DnsPacket * packet, unsigned char ** bufferArgument)
{
	char * name = new char[256];
	bzero(name, 256);
	
	unsigned char * buffer = *bufferArgument;
	
	bool first = true;
	bool argumentPinned = false;
	int offset = 0;
	
	unsigned char len = buffer[0];
	buffer++;
	
	while (len != 0)
	{
		if (len & 0xC0)
		{
			*bufferArgument = buffer + 1;
			argumentPinned = true;
			
			len = len ^ 0xC0;
			buffer = packet->firstByte + len;
		}
		
		if (!first) {
			name[offset++] = '.';
		}
		
		for (int i = 0; i < len; i++)
		{
			name[offset++] = (char)buffer[i];
		}
		
		buffer += len;
		len = buffer[0];
		buffer++;
		first = false;
	}
	
	if (!argumentPinned) {
		*bufferArgument = buffer;
	}
	
	return name;
}

char * DnsParsing::encodeName(char * name, int * length)
{
	char * encoded = new char[strlen(name) + 2];
	
	int i = 0;
	int lastIndex = 0;
	int u = 1;
	
	while (name[i] != '\0')
	{
		if (name[i] == '.') {
			encoded[lastIndex] = u - lastIndex - 1;
			lastIndex = u;
			u++;
		} else {
			encoded[u++] = name[i];
		}
		
		i++;
	}
	
	encoded[lastIndex] = u - lastIndex - 1;
	encoded[u] = 0;
	
	*length = u + 1;
	
	return encoded;
}

void DnsParsing::encodeHeader(DnsHeader * header)
{
	header->id = htons(header->id);
	header->flags = htons(header->flags);
	header->answerCount = htons(header->answerCount);
	header->questionCount = htons(header->questionCount);
	header->authorityCount = htons(header->authorityCount);
	header->additionalCount = htons(header->additionalCount);
}

void DnsParsing::encodeResource(DnsResource * resource)
{
	DnsResourceRecord * record = resource;

	record->nType = htons(record->nType);
	record->nClass = htons(record->nClass);
	record->ttl = htons(record->ttl);
	record->length = htons(record->length);
}

void mcopy(unsigned char ** dest, const void * source, int length) {
	memcpy(*dest, source, length);
	*dest += length;
}

unsigned char * DnsParsing::encodePacket(DnsPacket* packet, int * length)
{
	*length = sizeof(DnsHeader);
	
	char ** names = new char *[packet->answerCount + packet->questionCount];
	int * nameLengths = new int[packet->answerCount + packet->questionCount];
	
	int i, u, temp;
	
	u = 0;
	
	for (i = 0; i < packet->answerCount; i++) {
		names[u] = DnsParsing::encodeName(packet->answers[i].name, &nameLengths[u]);
		*length += nameLengths[u] + packet->answers[i].length + sizeof(DnsResourceRecord);
		u++;
	}
	
	for (i = 0; i < packet->questionCount; i++) {
		names[u] = DnsParsing::encodeName(packet->questions[i].name, &nameLengths[u]);
		*length += nameLengths[u] + packet->questions[i].length + sizeof(DnsResourceRecord);
		u++;
	}
	
	unsigned char * firstByte = new unsigned char[*length];
	unsigned char * entirePacket = firstByte;

	DnsParsing::encodeHeader(packet);
	mcopy(&entirePacket, packet, sizeof(DnsHeader));
	DnsParsing::decodeHeader(packet);
	
	u = 0;
	
	for (i = 0; i < packet->answerCount; i++) {
		temp = packet->answers[i].length;
		DnsParsing::encodeResource(&packet->answers[i]);
		
		mcopy(&entirePacket, names[u], nameLengths[u]);
		mcopy(&entirePacket, &packet->answers[i], sizeof(DnsResourceRecord));
		mcopy(&entirePacket, packet->answers[i].payload, temp);
		
		delete [] names[u];
		u++;
	}
	
	for (i = 0; i < packet->questionCount; i++) {
		temp = packet->questions[i].length;
		DnsParsing::encodeResource(&packet->questions[i]);
		
		mcopy(&entirePacket, names[u], nameLengths[u]);
		mcopy(&entirePacket, &packet->questions[i], sizeof(DnsResourceRecord));
		mcopy(&entirePacket, packet->questions[i].payload, temp);
		
		delete [] names[u];
		u++;
	}
	
	delete [] names;
	delete [] nameLengths;
	
	return firstByte;
}

void DnsParsing::releasePacket(DnsPacket * packet)
{
	int i;

	for (i = 0; i < packet->answerCount; i++) {
		if (packet->answers[i].name != 0) {
			delete [] packet->answers[i].name;
		}
		
		/*if (packet->answers[i].payload != 0) {
			delete [] packet->answers[i].payload;
		}*/
	}
	
	for (i = 0; i < packet->questionCount; i++) {
		if (packet->questions[i].name != 0) {
			delete [] packet->questions[i].name;
		}
		
		/*if (packet->questions[i].payload != 0) {
			delete [] packet->questions[i].payload;
		}*/
	}
	
	delete [] packet->answers;
	delete [] packet->questions;
	delete packet;
	
}
