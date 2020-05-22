#include "Server.h"

//Constructor
Server::Server(const char* ipAddress, int port, DmxMode dm)
{
	init(ipAddress, port, dm);
	setServerName("GENERIC", "SERVER");
	std::cout << "Server is ready to start." << std::endl;
}

Server::Server(const char* developer, const char* name, const char* ipAddress, int port, DmxMode dm)
{
	init(ipAddress, port, dm);
	setServerName(developer, name);
	std::cout << "Server ready to start." << std::endl;
}

//Destructor
Server::~Server()
{
	std::cout << "Server is closing..." << std::endl;
#ifdef _WIN32
	closesocket(sockfd);
	WSACleanup();
#endif
	free(inputbuffer);
	std::cout << "Server closed." << std::endl;
}

//Initialization sequence
void Server::init(const char* ipAddress, int port, DmxMode dm)
{
	std::cout << "Server is initializing..." << std::endl;

#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	std::cout << (WSAStartup(version, &wsaData) == 0 ?
		"Winsock started." : "Can't start Winsock.") << std::endl;
#endif

	running = false;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	udpPort = port;

	memset(&serverHint, '\0', sizeof(serverHint));
	serverHint.sin_family = AF_INET;
	serverHint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress, &(serverHint.sin_addr));

#ifdef _WIN32
	DWORD timeout = 3 * 1000;
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout)
#else
	struct timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	int on=1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
#endif

	std::cout << ((bind(sockfd, (struct sockaddr*) & serverHint, sizeof(serverHint)) < 0) ?
		"Can't bind socket!" : "Socket binded.") << std::endl;

	clientLength = sizeof(clientHint);
	memset(&clientHint, '\0', sizeof(clientHint));

	buffersize += 256 << (((size_t)dm - (size_t)Dmx256) >> 4);
	std::cout << "Buffer size: " << buffersize << std::endl;
	inputbuffer = (char*)malloc(buffersize);
	if (inputbuffer != NULL)
		memset(inputbuffer, '\0', buffersize);
}

void Server::setServerName(const char* developer, const char* name)
{
	memcpy(manufacture, developer, STRLENGTH - 1);
	memcpy(modelName, name, STRLENGTH);
}

void Server::start()
{
	std::cout << "Server is starting..." << std::endl;
	running = true;
	listenLoop = std::thread([this]() {receive(); });
	discoveryLoop = std::thread([this]() {sendPoll(); });
	std::cout << "Server started." << std::endl << std::endl;
}

void Server::stop()
{
	if (running)
	{
		std::cout << "Server is about to stop..." << std::endl;
		running = false;
		listenLoop.join();
		discoveryLoop.join();
		std::cout << "Server stopped." << std::endl;
	}
	else
	{
		std::cout << "Can't stop. Server is not running." << std::endl;
	}
}

void Server::receive()
{
	pollReply = new SllcpPollReply(manufacture, modelName, Dmx512, true, true, DevServer, 0, 0, 0, 0, 0, 0);
	HEXDUMP("rEpLy", pollReply);

	while (running)
	{
		delay(20);
		std::cout << (recvfrom(sockfd, inputbuffer, buffersize, 0, (struct sockaddr*) &clientHint, &clientLength) < 0 ?
			"Error receiving from " : "Packet received from ");
		inet_ntop(AF_INET, &clientHint.sin_addr, clientIp, 16);

		std::cout << clientIp << ":" << std::endl;

		uint8_t opCode = inputbuffer[8] & 0xff;

		std::cout << "[" << SLLCP::getOpName(opCode) << "] " << SLLCP::getOpDesc(opCode) << std::endl;

		switch (opCode)
		{
		//SLLCP.Poll received. Sending SLLCP.PollReply.
		case OPPOLL:
			Send(clientIp, pollReply);
			break;
		case OPPOLLREPLY:
		{
			SllcpPollReply* replyReceived = new SllcpPollReply();
			memcpy(replyReceived, inputbuffer, sizeof(SllcpPollReply));

			std::cout << "> Model:\t" << replyReceived->Manufacture << " - " << replyReceived->ModelName << std::endl;
			std::cout << "> Mode:\t\t" << "Dmx-" << pow(2, 8 + ((replyReceived->Flags & 0x30) >> 4)) << std::endl;
			std::cout << "> Wifi:\t\t" << ((replyReceived->Flags & 0x08) > 0 ? "yes" : "no") << std::endl;
			std::cout << "> Ethernet:\t" << ((replyReceived->Flags & 0x04) > 0 ? "yes" : "no") << std::endl;
			std::cout << "> Type:\t\t";
			switch (replyReceived->Flags & 0x03)
			{
			case DevController:
				std::cout << "Controller" << std::endl;
				break;
			case DevServer:
				std::cout << "Server" << std::endl;
				break;
			case DevNode:
				std::cout << "Node" << std::endl;
				break;
			case DevVisualizer:
				std::cout << "Visualizer" << std::endl;
				break;
			}
			std::cout << "> Interfaces:" << std::endl;
			std::cout << "  - DMX:\t input: " << ((replyReceived->InterfaceCnt[0] & 0xf0) >> 4);
			std::cout << " / output: " << (replyReceived->InterfaceCnt[0] & 0x0f) << std::endl;
			std::cout << "  - MIDI:\t input: " << ((replyReceived->InterfaceCnt[1] & 0xf0) >> 4);
			std::cout << " / output: " << (replyReceived->InterfaceCnt[1] & 0x0f) << std::endl;
			std::cout << "  - ILDA:\toutput: " << ((replyReceived->InterfaceCnt[2] & 0xf0) >> 4) << std::endl;
			std::cout << "  - LED strip:\toutput: " << (replyReceived->InterfaceCnt[2] & 0x0f) << std::endl;

			delete replyReceived;
			break;
		}
		case OPOUTDMX512:
			std::cout << "DMX-512 received." << std::endl;
			break;
		}
		memset(clientIp, '\0', 16);
		memset(inputbuffer, '\0', buffersize);
	}

	delete pollReply;
}

void Server::send(const char* ipAddress, char* input, int len)
{
	if (running)
	{
		sockaddr_in target;
		memset(&target, '\0', sizeof(target));
		target.sin_family = AF_INET;
		target.sin_port = htons(udpPort);
		inet_pton(AF_INET, ipAddress, &(target.sin_addr));

		int sendOk = sendto(sockfd, input, len, 0, (sockaddr*)&target, sizeof(target));
		if (sendOk == -1)
#ifdef _WIN32
			std::cout << "Error: " << WSAGetLastError() << std::endl;
#else
			fprintf(stderr, "socket() failed: %s\n", strerror(errno));
#endif
		else
			std::cout << SLLCP::getOpName(input[8]) << " message sent." << std::endl;
	}
	else
	{
		std::cout << "Can't send. Server is not running." << std::endl;
	}
}

bool Server::isRunning()
{
	return running;
}

void Server::sendBroadcast(char* input, int len)
{
	std::cout << "BROADCASTING: ";
	send(BROADCASTADDR, input, len);
}

void Server::sendPoll()
{
	SllcpPoll* pollReq = new SllcpPoll();

	while (running)
	{
		delay(5000);
		Broadcast(pollReq);
	}

	delete pollReq;
	
	std::cout << "Poll loop stopped." << std::endl;
}
