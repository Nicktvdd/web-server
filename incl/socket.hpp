#ifndef SOCKET_HPP
# define SOCKET_HPP
# include "library.hpp"
# define DEFAULTPORT 8080
# define DEFAULTSERVNAME "webserv" 

class listeningSocket
{
private:
	int serverFd;
	struct sockaddr_in address;
	int port;
	int newSocket;
public:
	listeningSocket();
	listeningSocket(int portNum);
	~listeningSocket();
	listeningSocket(const listeningSocket &var);
	listeningSocket& operator=(const listeningSocket &var);
	struct sockaddr_in getAddress();
	int getServerFd();
	int getPortNum();
};


#endif