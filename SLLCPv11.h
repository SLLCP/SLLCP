#pragma once

#define FNV1A64PRIME       0x00000100000001B3
#define FNV1A64OFFSETBASIS 0xcbf29ce484222325

struct IFnv1a
{
private:
	uint64_t algo(uint8_t*, int);
public:
	virtual void hash() = 0;
};

#pragma region "Laser messages."
#pragma pack(push, 1)
struct SllcpOutLaser : public IFnv1a
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTLASER;
	
	//uint8_t Padding[3] = { 0xaa, 0xbb, 0xcc };
	
	uint8_t FormatCode = 0;
	uint16_t NumberOfRecords = 0;

	uint32_t SeqId = 0;

	//TODO: ILDA-IDTF data

	uint64_t Fnv1a = 0;

	SllcpOutLaser(uint32_t);

	// Inherited via IFnv1a
	virtual void hash() override;
};
#pragma pack(pop)

//Close the lasers shutter. (Unicast.)
struct SllcpOutClose
{
	uint8_t ID[8] = HEADER;
	uint8_t OpCode = OPOUTCLOSE;

	uint8_t Close = 0xff; //Set all bits 1 for close, 0 for open (0 = open, 255 = close). Receiver should count the true bits, if (n > 1) closeShutter();

	SllcpOutClose(bool);
};
#pragma endregion

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

#pragma region "System messages."
//Requests the receiver to shutdown. (Unicast or Broadcast.)
struct SllcpShutdown
{
	uint8_t ID[8] = HEADER;
	uint8_t Opcode = OPSHUTDOWN;
};
#pragma endregion
