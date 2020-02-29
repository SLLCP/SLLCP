#pragma once
#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>

#include "OpCodes.h"

#define VERSION 0x10 //v1.0
#define HEADER { 'S', 'L', 'L', 'C', 'P', 'v', VERSION, '\0' };

#define UDPPORT 0x1936

#define FNV1A64PRIME       0x00000100000001B3
#define FNV1A64OFFSETBASIS 0xcbf29ce484222325

#define STRLENGTH 16

struct IFnv1a
{
public:
	virtual void hash() = 0;
};

#pragma region "Discover messages."
typedef enum
{
	DevController,
	DevServer,
	DevNode,
	DevVisualizer
} DeviceCode;

//Discover available devices. (Must be broadcast message!)
struct SllcpPoll
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPPOLL;
};

//Reply for the discover message. (Unicast.)
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
	inline constexpr static size_t arrsize = 256 << (((size_t)dm - (size_t)Dmx256) >> 4);
	static_assert((size_t)dm >= (size_t)Dmx256 && (size_t)dm <= (size_t)Dmx2048);

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

#pragma region "Laser messages."
//Colored dotes for lasers. (Unicast.)
struct SllcpOutLaser : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTLASER;

	uint32_t SeqId = 0;

	//TODO: ILDA-IDTF data

	uint64_t Fnv1a = 0;

	SllcpOutLaser(uint32_t);

	// Inherited via IFnv1a
	virtual void hash() override;
};

//Close the lasers shutter. (Unicast.)
struct SllcpOutClose
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTCLOSE;

	uint8_t Close = 0xff; //Set all bits 1 for close, 0 for open (0 = open, 255 = close). Receiver should count the true bits, if (n > 1) closeShutter();

	SllcpOutClose(bool);
};
#pragma endregion

//RGBWAU color codes and frequency for LED strip. (Unicast.)
struct SllcpOutStrip
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTSTRIP;

	uint8_t SeqId = 0;

	uint8_t Master = 0;
	uint8_t Color[6] = { 0 }; // R, G, B, W, A, U
	uint16_t TimeMs[4] = { 0 }; // RisingEdge, HighLevel, FallingEdge, LowLevel

	SllcpOutStrip(uint32_t);
};

//Reply for SllcpOut% messages. (Unicast.)
struct SllcpOutAck : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPOUTACK;

	uint32_t SeqId = 0;
	uint64_t Fnv1a = 0;
	// Inherited via IFnv1a
	virtual void hash() override;

	SllcpOutAck(uint32_t);
};

#pragma region "Getter and setter messages."
//Requests the receiver network configuration. (Unicast.)
struct SllcpGetIpConf
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPGETIPCONF;
};

//Requests the list of access points from the receivers view. (Unicast.)
struct SllcpGetApList
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPGETAPLIST;

	uint32_t SeqId = 0;

	SllcpGetApList(uint32_t);
};

//Requests the receiver to set his own DMX-mode. (Unicast.)
struct SllcpSetMode
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETMODE;

	uint8_t Length = OPOUTDMX256;

	SllcpSetMode(uint8_t);
};

//Requests the receiver to change his own network configuration. (Unicast.)
struct SllcpSetIpConf : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETIPADD;

	uint8_t IpAddress[4] = { 0 }; //If 32 pcs 0, than its a request for switch to DHCP.
	uint8_t SubnetMask[4] = { 0xff };

	uint64_t Fnv1a = 0;

	SllcpSetIpConf(uint8_t*, uint8_t*);

	// Inherited via IFnv1a
	virtual void hash() override;
};

//Requests the receiver to use the sent Access Point configuration. (Unicast.)
struct SllcpSetAp : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETWIFIAP;

	uint8_t Ssid[100] = { 0 };
	uint8_t password[12] = { 0 };

	uint64_t Fnv1a = 0;

	SllcpSetAp(uint8_t*, uint8_t*);

	// Inherited via IFnv1a
	virtual void hash() override;
};

//Reply for SllcpGetApList and SllcpSetAp messages. (Unicast.)
struct SllcpApReply : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPAPREPLY;

	uint32_t SeqId = 0;
	//TODO: dump all data from the AP list, and calculate a length for that
	uint64_t Fnv1a = 0;

	SllcpApReply(uint32_t);

	// Inherited via IFnv1a
	virtual void hash() override;
};
#pragma endregion

#pragma region "System messages."
//Requests the receiver to restart. (Unicast.)
struct SllcpRestart
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPRESTART;
};

//Requests the receiver to shutdown. (Unicast or Broadcast.)
struct SllcpShutdown
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPSHUTDOWN;
};

//The sender announces his intention to leave. (Broadcast.)
struct SllcpDisconnect
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPDISCONN;
};
#pragma endregion