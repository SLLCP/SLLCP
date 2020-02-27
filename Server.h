#pragma once

#include <stdio.h>
#include <stdlib.h>
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
#endif

#define BUFFERSIZE 30

using namespace std;

class Server
{
private:
	char buffer[BUFFERSIZE];
	int sockfd;
	struct sockaddr_in si_me, si_other;
	socklen_t addr_size;
	bool running;

	thread listenLoop, discoveryLoop;

	void sendPoll();

public:
	Server(const char*, int);
	~Server();

	void start();
	void stop();

	void receive();
	void send();
};

