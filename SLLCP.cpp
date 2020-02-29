#include "SLLCP.h"

SllcpPollReply::SllcpPollReply(const char* manufacture, const char* modelName, uint8_t length, bool hasWiFiIf, bool hasEthIf, DeviceCode dc, uint8_t dmxIn, uint8_t dmxOut, uint8_t midiIn, uint8_t midiOut, uint8_t laserOut, uint8_t stripOut)
{
	if (dmxIn > 15 || dmxOut > 15 || midiIn > 15 || midiOut > 15 || laserOut > 15 || stripOut > 15)
		throw std::invalid_argument("Max 15 interface supported.");

	memcpy(Manufacture, manufacture, STRLENGTH - 2);
	memcpy(ModelName, modelName, STRLENGTH - 1);

	Flags = length | hasWiFiIf << 3 | hasEthIf << 2 | dc;

	InterfaceCnt[0] = dmxOut | dmxIn << 4;
	InterfaceCnt[1] = midiOut | midiIn << 4;
	InterfaceCnt[2] = stripOut | laserOut << 4;
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

SllcpOutStrip::SllcpOutStrip(uint32_t seqId)
{
	SeqId = seqId;
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