#pragma once

#include <algorithm>
#include <vector>

#define UDPPORT 0x1936
#define STRLENGTH 16

#pragma region "Discover messages."
//Discover available devices. (Must be broadcast message!)
struct SllcpPoll
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPPOLL;
};

//Reply for the discover message. (Unicast.)
#pragma pack(push, 1)
struct SllcpPollReply
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPPOLLREPLY;

	uint8_t Manufacture[STRLENGTH - 1] = { 0 };
	uint8_t ModelName[STRLENGTH] = { 0 };
	uint8_t Flags = 0; // Bit 7-6: Reserved (should be 1), Bit 5-4: DmxLength, Bit 3: HasWiFi, Bit 2: HasEthernet; Bit 1-0: DeviceCode
	uint8_t InterfaceCnt[3] = { 0, 0, 0 }; //Byte 0: Bit 7-4: DmxIn, Bit 3-0: DmxOut; Byte 1: Bit 7-4: MidiIn, Bit 3-0: MidiOut; Byte 2: Bit 7-4: LaserOut, Bit 3-0: StripOut;

	SllcpPollReply() {};
	SllcpPollReply(const char*, const char*, uint8_t, bool, bool, DeviceCode, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
};
#pragma pack(pop)
#pragma pack(push, 1)
struct SllcpPollResults
{
	uint8_t ID[8];
	uint8_t OpCode = OPPOLLRESULTS;
	
	uint8_t Length = 0; //Max. 23
	uint8_t PacketsLeft = 0;
	
	SllcpPollReply List[23];
	
	uint8_t EndOfPacket = 0xff;
	
	SllcpPollResults();
	
	static SllcpPollResults *PollResultsFactory();
}
#pragma pack(pop)
#pragma endregion

#pragma region "Dmx messages."
//One universe of DMX data. (Unicast.)
enum DmxMode
{
	Dmx256 = 0xc0,
	Dmx512 = 0xd0,
	Dmx1024 = 0xe0,
	Dmx2048 = 0xf0
};

#pragma pack(push, 1)
template <DmxMode dm>
struct  SllcpOutDmx
{
	constexpr static size_t arrsize = 256 << (((size_t)dm - (size_t)Dmx256) >> 4);
	//static_assert((size_t)dm >= (size_t)Dmx256 && (size_t)dm <= (size_t)Dmx2048);

	uint8_t ID[8] = HEADER;
	uint8_t OpCode = dm; //Bit 7-6: OpOutDmx, Bit 5-4: Length, Bit 3-0: Interface selector

	uint8_t padding[3] = { 0xaa, 0xbb, 0xcc }; //Can be anything, it's just a padding.

	uint32_t SeqId = 0;
	uint32_t Forward = 0; //If 0.0.0.0, than the receiver is the addressee, else it's an IP address to forward.

	uint8_t Data[arrsize] = { 0 };

	SllcpOutDmx(uint32_t);

	void setChannel(int, uint8_t);
};
#pragma pack(pop)

template<DmxMode dm>
inline SllcpOutDmx<dm>::SllcpOutDmx(uint32_t seqId)
{
	SeqId = seqId;
	for (size_t i = 0; i < arrsize; i++)
		setChannel(i, i % 256);
}
template<DmxMode dm>
inline void SllcpOutDmx<dm>::setChannel(int ch, uint8_t value)
{
	if (ch < 0 || ch >= arrsize)
		throw std::invalid_argument("Channel not exist.");
	Data[ch] = value;
}
#pragma endregion

#pragma region "System messages."
//Requests the receiver to restart. (Unicast.)
struct SllcpRestart
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPRESTART;
};

//The sender announces his intention to leave. (Broadcast.)
struct SllcpDisconnect
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPDISCONN;
};
#pragma endregion
