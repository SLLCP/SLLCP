#include "SLLCPv12.h"

SllcpOutStrip::SllcpOutStrip(uint32_t seqId)
{
	SeqId = seqId;
}

SllcpGetApList::SllcpGetApList(uint32_t seqId)
{
	SeqId = seqId;
}

SllcpSetMode::SllcpSetMode(uint8_t length)
{
	Length = length;
}

SllcpSetIpConf::SllcpSetIpConf(uint8_t* ipAdd, uint8_t* sMask)
{
	//TODO: Copy arrays
}

void SllcpSetIpConf::hash()
{
	//TODO: generate HASH
}

SllcpSetAp::SllcpSetAp(uint8_t* ssid, uint8_t* pass)
{
	//TODO: Copy arrays
}

void SllcpSetAp::hash()
{
	//TODO: generate HASH
}

SllcpApReply::SllcpApReply(uint32_t seqId)
{
	SeqId = seqId;
}

void SllcpApReply::hash()
{
	//TODO: generate HASH
}
