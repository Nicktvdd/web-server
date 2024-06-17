#include "../incl/server.hpp"

// Default constructor
Server::Server()
{
	// Initialize member variables with default values
	this->servName = "defaultserv";
	this->error404Dir = DEFAULT404DIR;
	this->cgiExt = "";
	this->cgiPath = "";
	this->client_max_body_size = 0;
	this->numOfPorts = 0;
}

// Destructor
Server::~Server()
{
	std::cout << "Deleting server" << std::endl;
}

// Copy constructor
Server::Server(const Server &var)
{
	// Copy member variables from the source object
	this->servName = var.servName;
	this->rootDir = var.rootDir;
	this->error404Dir = var.error404Dir;
	this->cgiExt = var.cgiExt;
	this->cgiPath = var.cgiPath;
	this->client_max_body_size = var.client_max_body_size;
	this->numOfPorts = var.numOfPorts;
	this->ports = var.ports;
	this->listeners = var.listeners;
}

// Assignment operator
Server &Server::operator=(const Server &var)
{
	if (this != &var)
	{
		// Copy member variables from the source object
		this->servName = var.servName;
		this->rootDir = var.rootDir;
		this->error404Dir = var.error404Dir;
		this->cgiExt = var.cgiExt;
		this->cgiPath = var.cgiPath;
		this->client_max_body_size = var.client_max_body_size;
		this->numOfPorts = var.numOfPorts;
		this->ports = var.ports;
		this->listeners = var.listeners;
	}
	return (*this);
}

// Print server information
void Server::print(void)
{
	std::cout << "server name = " << servName << std::endl;
	std::cout << "(relative) root dir = " << rootDir << std::endl;
	std::cout << "error dir = " << error404Dir << std::endl;
	std::cout << "cgi path and ext  = " << cgiPath << " and " << cgiExt << std::endl;
	std::cout << "Num of ports = " << numOfPorts << std::endl;
	std::cout << "ports are " << std::endl;
	for (int i = 0; i < numOfPorts; i++)
	{
		std::cout << ports.at(i) << std::endl;
	}
}

// Get server name
std::string Server::getServerName(void)
{
	return this->servName;
}

// Set server name
void Server::setServerName(std::string name)
{
	this->servName = name;
}

// Set root directory
void Server::setRootDir(std::string dir)
{
	this->rootDir = dir;
}

// Set error directory
void Server::setErrorDir(std::string dir)
{
	this->error404Dir = dir;
}

// Set CGI extension
void Server::setCGIExt(std::string ext)
{
	if (this->cgiExt.empty())
		this->cgiExt = ext;
}

// Set CGI path
void Server::setCGIPath(std::string path)
{
	this->cgiPath = path;
}

// Set maximum client body size
void Server::setClientBodySize(std::string size)
{
	this->client_max_body_size = std::stoi(size);
}

// Get maximum client body size
int Server::getClientBodySize(void)
{
	return this->client_max_body_size;
}

// Get CGI path
std::string Server::getCGIPath(void)
{
	return this->cgiPath;
}

// Get CGI extension
std::string Server::getCGIExt(void)
{
	return this->cgiExt;
}

// Get MIME type based on file extension
std::string Server::getMIMEType(std::string fileExt)
{
	if (fileExt.compare(".html") == 0 || fileExt.compare(".htm") == 0)
	{
		return "text/html";
	}
	else if (fileExt.compare(".txt") == 0)
	{
		return "text/plain";
	}
	else if (fileExt.compare(".jpg") == 0 || fileExt.compare(".jpeg") == 0)
	{
		return "image/jpeg";
	}
	else if (fileExt.compare(".png") == 0)
	{
		return "image/png";
	}
	else
	{
		return "application/octet-stream";
	}
}

// Log text to a file
void Server::log(std::string text)
{
	std::ofstream logfile;
	logfile.open("logfile.txt", std::ofstream::app);
	if (logfile.is_open() == 0)
	{
		std::cout << "Failed to open logfile.txt" << std::endl;
		return;
	}
	time_t rawtime;
	struct tm *timeinfo;
	char timeBuffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeBuffer, 80, "%T %d:%m:%Y", timeinfo);
	logfile << "----------------------------------------------------------------------------------------------------" << std::endl;
	logfile << "New entry in log, at time " << timeBuffer << std::endl;
	logfile << text;
	logfile << std::endl
			<< std::endl;
	logfile.close();
}

// Add a port to the server
void Server::addPort(int port)
{
	int i;
	for (i = 0; i < numOfPorts; i++)
	{
		if (ports.at(i) == port)
			break;
	}
	if (i == numOfPorts)
	{
		ports.push_back(port);
		numOfPorts++;
	}
}

// Get the number of ports
int Server::getNumOfPorts(void)
{
	return this->numOfPorts;
}

// Set socket to non-blocking mode
void setnonblocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

// Create listening sockets for all ports
void Server::makeSocketList()
{
	for (int i = 0; i < numOfPorts; i++)
	{
		listeningSocket newSocket(ports.at(i));
		setnonblocking(newSocket.getServerFd());
		listeners.push_back(newSocket);
	}
}

// Get the root directory
std::string Server::getRootDir()
{
	return this->rootDir;
}