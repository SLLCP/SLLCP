#pragma once

#include <cstdint>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <string.h>

#define VERSION 0x10 //v1.0 = 0x10; v1.1 = 0x11; etc.
#define HEADER { 'S', 'L', 'L', 'C', 'P', 'v', VERSION, '\0' };

#include "SLLCPv10.h"
#if VERSION > 0x10
  #include "SLLCPv11.h"
  #if VERSION > 0x11
    #include "SLLCPv12.h"
  #endif
#endif
#include "OpCodes.h"

typedef enum
{
	DevController,
	DevServer,
	DevNode,
	DevVisualizer
} DeviceCode;
