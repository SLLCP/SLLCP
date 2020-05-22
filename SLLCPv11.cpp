#include "SLLCPv11.h"

uint64_t IFnv1a::algo(uint8_t* raw, int len)
{
	uint64_t hash = FNV1A64OFFSETBASIS;
	
	for (int i = 0; i < len; i++)
	{
		hash ^= raw[i];
		hash *= FNV1A64PRIME;
	}
	
	return hash;
}

SllcpOutLaser::SllcpOutLaser(uint32_t seqId)
{
	SeqId = seqId;
}

void SllcpOutLaser::hash()
{
	//TODO: generate HASH
}

SllcpOutClose::SllcpOutClose(bool close)
{
	Close = close ? 0xff : 0x00;
}

void SllcpOutAck::hash()
{
	//TODO: generate HASH
	//Not necessary for DMX, in this case can be full 0;
}

SllcpOutAck::SllcpOutAck(uint32_t seqId)
{
	SeqId = seqId;
}
