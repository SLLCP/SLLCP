#pragma once

#include <iostream>
#include <thread>

#define delay(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms));

#define HEXDUMP(description, data) Utils::hexDump(description, data, sizeof(*data))

class Utils
{
public:
	static void hexDump(const char*, const void*, const int);
};

