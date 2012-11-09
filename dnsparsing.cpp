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
	header->id = htons(header->id);
	header->flags = htons(header->flags);
	header->answerCount = htons(header->answerCount);
	header->questionCount = htons(header->questionCount);
	header->resourceCount = htons(header->resourceCount);
	header->additionalCount = htons(header->additionalCount);
}

unsigned char * DnsParsing::decodeResource(DnsPacket * packet, DnsResource * resource, unsigned char * buffer)
{
	resource->name = DnsParsing::decodeName(packet, &buffer);
	
	DnsResourceRecord * record = resource;
	memcpy(record, buffer, sizeof(DnsResourceRecord));
	
	record->nType = htons(record->nType);
	record->nClass = htons(record->nClass);
	record->ttl = htons(record->ttl);
	record->length = htons(record->length);
	
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

void DnsParsing::releasePacket(DnsPacket * packet)
{
	int i;

	for (i = 0; i < packet->questionCount; i++) {
		cout << "Question: " << packet->questions[i].name << endl;
	}
	
	for (i = 0; i < packet->answerCount; i++) {
		delete [] packet->answers[i].name;
	}
	
	for (i = 0; i < packet->questionCount; i++) {
		delete [] packet->questions[i].name;
	}
	
	delete [] packet->answers;
	delete [] packet->questions;
	delete packet;
	
}
