#include <iostream>

#include "SLLCP.h"
#include "Server.h"
#include "Utils.h"

int main()
{
	std::cout << "Stage Lighting and Laser Control Protocol" << std::endl;
	std::cout << "-----------------------------------------" << std::endl << std::endl;

	char serverIp[16] = "192.168.1.202";

	Server* server = new Server(serverIp, 0x1936, Dmx512);
	server->start();

	//The first Poll sent after 5 seconds. Let's simulate a respond from a Node.
	delay(6000);
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
	server->stop();
	delete server;
}