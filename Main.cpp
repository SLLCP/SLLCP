#include <iostream>

#include "SLLCP.h"
#include "Server.h"
#include "Utils.h"

#ifndef _WIN32
	#include <csignal>
#endif

Server* server;

#ifdef _WIN32
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
		server->stop();
		return TRUE;
		break;
	default:
		return FALSE;
		break;
	}
}
#else
void my_handler(int s) {
	printf("\nCaught signal %d\n", s);
	server->stop();
	//exit(1);
}
#endif

int main()
{
	std::cout << "Stage Lighting and Laser Control Protocol" << std::endl;
	std::cout << "-----------------------------------------" << std::endl << std::endl;

#ifdef _WIN32
	if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
	{
		printf("\nThe Control Handler is installed.\n");
#else
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
#endif

		char serverIp[16] = "192.168.1.10";

		server = new Server("SERVER", serverIp, serverIp, 0x1936, Dmx512);
		server->start();

		//The first Poll sent after 5 seconds. Let's simulate a respond from a Node.
		/*delay(6000);
		char
			manufacture[STRLENGTH - 1] = "Nejlontacsko",
			modelName[STRLENGTH] = "LightBox-01";
		SllcpPollReply* pollReply = new SllcpPollReply(manufacture, modelName, OPOUTDMX512, true, false, DevNode, 1, 3, 1, 1, 0, 0);
		server->Send(serverIp, pollReply);
		delete pollReply;

		//Create some traffic
		delay(1500);
		SllcpOutDmx<Dmx512>* dmxOut = new SllcpOutDmx<Dmx512>(10);
		HEXDUMP("DMXDATA", dmxOut);
		std::cout << std::endl << "\tDMXDATA length: " << sizeof(*dmxOut) << std::endl << std::endl;
		server->Send(serverIp, dmxOut);
		delete dmxOut;

		//Let the server run for a half minute, than stop
		delay(30000);
		server->stop();*/
		while (server->isRunning())
			delay(1000);
#ifdef _WIN32
	}
	else
	{
		printf("\nERROR: Could not set control handler");
		return 1;
	}
#endif

	delete server;

	return 0;
}
