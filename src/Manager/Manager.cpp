#include "../../incl/Manager.hpp"

Manager::Manager()
{
}

Manager::~Manager()
{
}

Manager::Manager(const Manager &var)
{
	this->pids = var.pids;
	this->serverList = var.serverList;
	this->serverIndex = var.serverIndex;
	this->fds = var.fds;
	this->cgiOnGoing = var.cgiOnGoing;
	this->data = var.data;
	this->fdsTimestamps = var.fdsTimestamps;
}

Manager &Manager::operator=(const Manager &var)
{
	if (this != &var)
	{
		this->pids = var.pids;
		this->serverList = var.serverList;
		this->serverIndex = var.serverIndex;
		this->fds = var.fds;
		this->cgiOnGoing = var.cgiOnGoing;
		this->data = var.data;
		this->fdsTimestamps = var.fdsTimestamps;
	}
	return (*this);
}

void setNonBlocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// New connection
bool Manager::acceptNewConnections(size_t index)
{
	for (int j = 0; j < serverList.size(); j++)
	{
		for (int k = 0; k < serverList.at(j).getNumOfPorts(); k++)
		{
			if (fds[index].fd == serverList.at(j).listeners.at(k).getServerFd())
			{
				std::cout << "new connection" << std::endl;
				while (true)
				{
					struct sockaddr_in clientAddr;
					socklen_t clientAddrLen = sizeof(clientAddr);
					int clientFd = accept(fds[index].fd, (struct sockaddr *)&clientAddr, &clientAddrLen);
					if (clientFd < 0)
					{
						if (errno == EWOULDBLOCK || errno == EAGAIN)
						{
							// No more clients to accept
							break;
						}
						else
						{
							std::cerr << "Accept error: " << strerror(errno) << std::endl;
							break;
						}
					}
					setNonBlocking(clientFd);
					// Add the new client socket to the poll list
					struct pollfd clientPfd;
					clientPfd.fd = clientFd;
					clientPfd.events = POLLIN;
					fds.push_back(clientPfd);
					fdsTimestamps.push_back(time(NULL));
					cgiOnGoing.push_back(0);
					serverIndex.push_back(std::make_pair(clientFd, j));
					std::cout << "here, i, j, k are " << index << " " << j << " " << k << std::endl;
				}
				return true; // Return true as soon as a new connection is accepted
			}
		}
	}
	return false; // Return false if no new connection was accepted
}

// Run
void Manager::run(std::string configFile)
{
	if (readConfig(configFile) == 0)
	{
		std::cerr << "ERROR reading config file" << std::endl;
		return;
	}
	std::cout << "--------------------------------------------------" << std::endl
			  << "number of servers = " << serverList.size() << std::endl;

	setupPollingforServers();

	while (true)
	{
		handlePolling();
	}
}
