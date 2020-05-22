#include "SLLCPv10.h"

SllcpPollReply::SllcpPollReply(const char* manufacture, const char* modelName, uint8_t* length, bool hasWiFiIf, bool hasEthIf, DeviceCode dc, uint8_t dmxIn, uint8_t dmxOut, uint8_t midiIn, uint8_t midiOut, uint8_t laserOut, uint8_t stripOut)
{
	if (dmxIn > 15 || dmxOut > 15 || midiIn > 15 || midiOut > 15 || laserOut > 15 || stripOut > 15)
		throw std::invalid_argument("Max 15 interface supported.");

	memcpy(Manufacture, manufacture, STRLENGTH - 2);
	memcpy(ModelName, modelName, STRLENGTH - 1);

	Flags = *length | hasWiFiIf << 3 | hasEthIf << 2 | dc;

	InterfaceCnt[0] = dmxOut | dmxIn << 4;
	InterfaceCnt[1] = midiOut | midiIn << 4;
	InterfaceCnt[2] = stripOut | laserOut << 4; //Must be Zeros in v1.0
}

SllcpPollResults::SllcpPollResults(SllcpPollReply* list, uint8_t len, uint8_t left) :
  List(list), Length(len), PacketsLeft(left) {}

void SllcpPollResults::PollResultsFactory(vector<SllcpPollResults*> results, SllcpPollReply* list, int len)
{
	SllcpPollReply tmp[23];
	
	//Divide the list by 23 items and fill into packets
	for (int i = 0; i < len / 23; i++)
	{
		memcpy(tmp, list + i * 23, sizeof(SllcpPollReply) * 23);
		results.push_back(new SllcpPollResults(tmp, 23, len / 23 - i));
	}
	
	//Make a packet for the rest
	memcpy(tmp, list - len % 23, sizeof(SllcpPollReply) * len % 23);
	results.push_back(new SllcpPollResults(tmp, len % 23, 0));
}
