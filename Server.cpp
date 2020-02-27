#include "Server.h"
#include "SLLCP.h"

Server::Server(const char* ipAddress, int port)
{
#ifdef _WIN32
	WORD versionWanted = MAKEWORD(1, 1);
	WSADATA wsaData;
	WSAStartup(versionWanted, &wsaData);
#endif

	running = false;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&si_me, '\0', sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress, &(si_me.sin_addr));

	bind(sockfd, (struct sockaddr*) & si_me, sizeof(si_me));
	addr_size = sizeof(si_other);
}

Server::~Server()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

void Server::start()
{
	running = true;
	listenLoop = thread([this]() {receive(); });
	discoveryLoop = thread([this]() {sendPoll(); });
}

void Server::stop()
{
	running = false;
	listenLoop.join();
	discoveryLoop.join();
}

void Server::receive()
{
	while (running)
	{
		recvfrom(sockfd, buffer, BUFFERSIZE, 0, (struct sockaddr*) & si_other, &addr_size);
		printf("[+]Data Received: %s", buffer);
		memset(buffer, 0, BUFFERSIZE);
	}
}

void Server::send()
{
}

void Server::sendPoll()
{
	SllcpPoll* pollReq = new SllcpPoll();

	while (running)
	{
		this_thread::sleep_for(chrono::milliseconds(5000));
		send();
	}

	delete pollReq;
}