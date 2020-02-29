#include "OpCodes.h"

std::string SLLCP::getOpName(uint8_t code)
{
	return OpCodeNames[code];
}

std::string SLLCP::getOpDesc(uint8_t code)
{
	return OpCodeDescriptions[code];
}
