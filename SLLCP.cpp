#include <iostream>

#define VERSION 1
#define HEADER { 'S', 'L', 'L', 'C', 'P', 'v', VERSION, 0 };

#define UDPPORT 0x1936

//Operation Codes
#define OPTESTMSG	0x00
#define OPRESTART	0x01
#define OPSHUTDOWN	0x02
#define OPDISCONN	0x03
#define OPPOLL      0x10
#define OPPOLLREPLY 0x11
#define OPOUTLASER	0x21
#define OPOUTCLOSE	0x20
#define OPOUTSTRIP	0x22
#define OPOUTACK	0x2f
#define OPGETIPCONF	0x30
#define OPGETAPLIST	0x31
#define OPSETMODE	0x40
#define OPSETIPADD	0x41
#define OPSETWIFIAP	0x42
#define OPSETACK	0x4f
#define OPIPREPLY	0x51
#define OPAPREPLY	0x52
#define OPOUTDMX	0xc0

enum DeviceCode
{
	DevController,
	DevServer,
	DevNode,
	DevVisualizer
};

enum DmxMode
{
	DMX256 = 0xc0,
	DMX512 = 0xd0,
	DMX1024 = 0xe0,
	DMX2048 = 0xf0,
};

#pragma region "Discover messages."
//Discover available devices. (Must be broadcast message!)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPPOLL;
} SllcpPoll;

//Reply for the discover message. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPPOLLREPLY;

	uint8_t Manufacture[7];
	uint8_t ModelName[8];
	uint8_t Flags; // Bit 7: Reserved (should be 0), Bit 4-5: DmxLength, Bit 3: HasWiFi, Bit 2: HasEthernet; Bit 1-0: DeviceCode
	uint8_t InterfaceCnt[3]; //Byte 0: Bit 7-4: DmxIn, Bit 3-0: DmxOut; Byte 1: Bit 7-4: MidiIn, Bit 3-0: MidiOut; Byte 2: Bit 7-4: LaserOut, Bit 3-0: StripOut;
} SllcpPollReply;
#pragma endregion

#pragma region "Dmx messages."
//One universe of DMX-256 data. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = DMX256; //Bit 7-6: OpOutDmx, Bit 5-4: Length, Bit 3-0: Interface selector

	uint32_t SeqId;

	uint8_t Data[256];
} SllcpOutDmx;

//One universe of DMX-512 data. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = DMX512; //Bit 7-6: OpOutDmx, Bit 5-4: Length, Bit 3-0: Interface selector

	uint32_t SeqId;

	uint8_t Data[512];
} SllcpOutDmx;

//One universe of DMX-1024 data. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = DMX1024; //Bit 7-6: OpOutDmx, Bit 5-4: Length, Bit 3-0: Interface selector

	uint32_t SeqId;

	uint8_t Data[1024];
} SllcpOutDmx;

//One universe of DMX-2048 data. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = DMX2048; //Bit 7-6: OpOutDmx, Bit 5-4: Length, Bit 3-0: Interface selector

	uint32_t SeqId;

	uint8_t Data[2048];
} SllcpOutDmx;
#pragma endregion

#pragma region "Laser messages."
//Colored dottes for lasers. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTLASER;

	uint32_t SeqId;
	uint16_t Crc16;

	//TODO: ILDA-IDTF datas
} SllcpOutLaser;

//Close the lasers shutter. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTCLOSE;

	uint8_t Close; //Set all bits 1 for close, 0 for open (0 = open, 255 = close). Receiver should count the true bits, if (n > 1) closeShutter();
} SllcpOutClose;
#pragma endregion

//RGBWAU color codes and frequency for LED strip. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTSTRIP;

	uint8_t Master;
	uint8_t Color[6]; // R, G, B, W, A, U
	uint16_t TimeMs[4]; // RisingEdge, HighLevel, FallingEdge, LowLevel
} SllcpOutStrip;

//Reply for SllcpOut% messages. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPOUTACK;

	uint32_t SeqId;
	uint16_t Crc16; //Not neccessary for DMX, in this case can be full 0;
} SllcpOutAck;

#pragma region "Getter and setter messages."
//Requests the receiver network configuration. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPGETIPCONF;
} SllcpGetIpConf;

//Requests the list of access points from the receivers view. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPGETAPLIST;
} SllcpGetApList;

//Requests the receiver to set his own DMX-mode. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETMODE;

	uint8_t Length;
} SllcpSetMode;

//Requests the receiver to change his own network configuration. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETIPADD;

	uint8_t IpAddress[4]; //If 32 pcs 0, than its a request for switch to DHCP.
	uint8_t SubnetMask[4];

} SllcpSetIpConf;

//Requests the receiver to use the sent Acces Point configuration. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPSETWIFIAP;

	uint8_t Ssid[100];
	uint8_t password[12];
} SllcpSetAp;

//Reply for Sllcp%IpConf messages. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPIPREPLY;

	uint8_t IpAddress[4];
	uint8_t SubnetMask[4];
} SllcpIpConfReply;

//Reply for SllcpGetApList and SllcpSetAp messages. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPAPREPLY;
	//TODO: dump all data from the AP lsit, and calculat a length for that
} SllcpApReply;
#pragma endregion

#pragma region "System messages."
//Requests the receiver to restart. (Unicast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPRESTART;
} SllcpRestart;

//Requests the receiver to shutdown. (Unicast or Broadcast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPSHUTDOWN;
} SllcpShutdown;

//The sender announces his intention to leave. (Broadcast.)
typedef struct
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPDISCONN;
} SllcpDisconnect;
#pragma endregion

int main()
{
    std::cout << "Hello World!" << std::endl;
	std::cout << "Stage Lighting and Laser Control Protocol" << std::endl;

	SllcpPoll poll;

	std::cout << poll.ID << std::endl;
}