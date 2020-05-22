#pragma once

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
