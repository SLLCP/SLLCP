#pragma once

#include <iostream>

#include <string.h>
#include <sys/types.h>
#include <thread>

#ifdef _WIN32
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <cmath>
#endif

#include "SLLCP.h"
#include "Utils.h"

#define BROADCASTADDR "192.168.1.255"

#define Send(ip, msg_struct) send(ip, reinterpret_cast<char*>(msg_struct), sizeof(*msg_struct))
#define Broadcast(msg_struct) sendBroadcast(reinterpret_cast<char*>(msg_struct), sizeof(*msg_struct))

class Server
{
private:
	char
		manufacture[STRLENGTH - 1] = { 0 },
		modelName[STRLENGTH] = { 0 },
		*inputbuffer,
		clientIp[16];
	int sockfd, udpPort, buffersize = 20;
	struct sockaddr_in serverHint, clientHint;
	socklen_t clientLength;
	bool running;

	std::thread listenLoop, discoveryLoop;

	SllcpPollReply* pollReply;

	void init(const char*, int, DmxMode);
	void setServerName(const char*, const char*);

	void sendBroadcast(char*, int);
	void sendPoll();

public:
	Server(const char*, int, DmxMode);
	Server(const char*, const char*, const char*, int, DmxMode);
	~Server();

	void start();
	void stop();

	void receive();
	void send(const char*, char*, int);

	bool isRunning();
};

