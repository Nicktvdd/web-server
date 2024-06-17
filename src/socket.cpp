#include "../incl/socket.hpp"

listeningSocket::listeningSocket()
{
	// Default constructor
}

int listeningSocket::createSocket(int portNum)
{
	std::cout << "Creating socket (with port num " << portNum << ")" << std::endl;
	this->port = portNum;

	// Create socket
	if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "ERROR: Failed to create socket - " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}

listeningSocket::listeningSocket(int portNum)
{
	if (createSocket(portNum) == -1)
		return;
	// Set socket options to reuse address
	const int enable = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
	{
		std::cout << "ERROR: Failed to set socket options - " << strerror(errno) << std::endl;
		// Don't close the socket here, because it is closed with closeInactiveConnections RIGHT??
		return;
	}

	// Set up server address
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(portNum);
	memset(address.sin_zero, '\0', sizeof(address.sin_zero));

	// Bind socket to the specified address
	if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cout << "ERROR: Failed to bind socket - " << strerror(errno) << std::endl;
		return;
	}

	// Start listening for incoming connections
	if (listen(serverFd, 10) < 0)
	{
		std::cout << "ERROR: Failed to listen - " << strerror(errno) << std::endl;
		return;
	}
}

listeningSocket::~listeningSocket()
{
	std::cout << "Deleting socket with portNum " << port << std::endl;
}

listeningSocket::listeningSocket(const listeningSocket &var)
{
	// Copy constructor
	this->serverFd = var.serverFd;
	this->address = var.address;
	this->port = var.port;
	this->newSocket = var.newSocket;
}

listeningSocket &listeningSocket::operator=(const listeningSocket &var)
{
	// Assignment operator
	if (this != &var)
	{
		this->serverFd = var.serverFd;
		this->address = var.address;
		this->port = var.port;
		this->newSocket = var.newSocket;
	}
	return (*this);
}

int listeningSocket::getPortNum()
{
	return this->port;
}

struct sockaddr_in listeningSocket::getAddress()
{
	return this->address;
}

int listeningSocket::getServerFd()
{
	return this->serverFd;
}
