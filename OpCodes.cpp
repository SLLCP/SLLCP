#include "OpCodes.h"

std::string SLLCP::getOpName(uint8_t code)
{
	return OpCodeNames[code];
	//return "ASD";
}

std::string SLLCP::getOpDesc(uint8_t code)
{
	return OpCodeDescriptions[code];
	//return "ASDF01";
}
