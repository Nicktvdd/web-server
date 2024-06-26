#include "../../incl/Manager.hpp"

// GET
void Manager::handleGet(std::string receivedData, std::vector<struct pollfd> fds, int i)
{
	std::string response;
	size_t start = receivedData.find('/');
	size_t end = receivedData.find(' ', start);
	std::string file = receivedData.substr(start + 1, end - start - 1);
	std::string fileExt = ".html";
	std::string fileName;

	if (file.find('.') != std::string::npos)
	{
		fileExt = file.substr(file.find('.'));
		fileName = file.substr(0, file.find('.'));
	}
	else
	{
		fileName = file;
	}

	std::cout << "File ext = " << fileExt << std::endl;

	if (fileExt.find("?") != std::string::npos)
	{
		end = fileExt.find("?");
		fileExt = fileExt.substr(0, end);
	}

	int index;

	for (index = 0; index < serverIndex.size(); index++)
	{
		if (serverIndex.at(index).first == fds[i].fd)
		{
			break;
		}
	}

	std::cout << "HERE!" << std::endl;
	std::cout << "File ext = " << fileExt << " and cgi ext = " << serverList.at(serverIndex.at(index).second).getCGIExt() << std::endl;
	std::cout << serverList.size() << " and " << serverIndex.size() << " and " << index << std::endl;

	if (fileExt.compare(serverList.at(serverIndex.at(index).second).getCGIExt()) == 0)
	{
		std::cout << "starting CGI" << std::endl;
		serverList.at(serverIndex.at(index).second).log(receivedData);
		handleCGI(receivedData, fds, i);
	}
	else
	{
		// for debugging
		std::cout << "here, making up a response, i = " << i << std::endl;
		serverList.at(serverIndex.at(index).second).log(receivedData);
		response = serverList.at(serverIndex.at(index).second).buildHTTPResponse(fileName, fileExt);

		if (serverList.at(serverIndex.at(index).second).getClientBodySize() != 0 && serverList.at(serverIndex.at(index).second).getClientBodySize() < response.length())
		{
			std::stringstream responseStream;
			std::cout << "response too large" << std::endl;
			std::string body = "ERROR 413 Request Entity Too Large";
			response = serverList.at(serverIndex.at(index).second).makeHeader(413, body.size());
			response.append(body);
		}

		send(fds[i].fd, response.c_str(), response.length(), 0);
	}
}

// Handle CGI
void Manager::handleCGI(std::string receivedData, std::vector<struct pollfd> fds, int i)
{
	std::string response;
	cgiOnGoing[i] = 1;
	for (int j = 0; j < serverIndex.size(); j++)
	{
		if (serverIndex.at(j).first == fds[i].fd)
		{
			if (serverList.at(serverIndex.at(j).second).getCGIExt().empty() || serverList.at(serverIndex.at(j).second).getCGIPath().empty())
			{
				std::cout << "ERROR, CGI not set" << std::endl;
				std::string body = "CGI not available";
				response = serverList.at(serverIndex.at(j).second).makeHeader(418, body.size());
				response.append(body);
				cgiOnGoing[i] = 0;
				send(fds[i].fd, response.c_str(), response.length(), 0);
			}
			else
			{
				std::cout << "Doing cgi" << std::endl;
				int pid;
				std::cout << "FORKING" << std::endl;
				pid = fork();
				if (pid < 0)
				{
					std::cout << "ERROR piderror" << std::endl;
					std::string body = "Internal server error";
					response = serverList.at(serverIndex.at(j).second).makeHeader(500, body.size());
					response.append(body);
					cgiOnGoing[i] = 0;
					send(fds[i].fd, response.c_str(), response.length(), 0);
				}
				else if (pid == 0)
				{
					std::string path = serverList.at(serverIndex.at(j).second).getCGIPath();
					std::string cmd = serverList.at(serverIndex.at(j).second).getRootDir();
					receivedData = receivedData.substr(receivedData.find("/") + 1);
					cmd.append(receivedData);
					if (cmd.find("?") != std::string::npos)
					{
						int end = cmd.find("?");
						cmd.erase(end);
					}
					std::cout << "path = " << path << std::endl;
					std::cout << "cmd = " << cmd << std::endl;
					std::string fName = serverList.at(serverIndex.at(j).second).getRootDir();
					fName.append("temp");
					fName.append(std::to_string(i));
					int fd = open(fName.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
					dup2(fd, 1);
					char *cmdArr[] = {const_cast<char *>(path.data()), const_cast<char *>(cmd.data()), NULL};
					execvp(cmdArr[0], cmdArr);
					exit(0);
				}
				else
				{
					std::cout << "pid is " << pid << std::endl;
					pids.push_back(std::make_pair(pid, i));
				}
			}
			break;
		}
	}
}

// POST
void Manager::handlePost(std::string receivedData, std::vector<struct pollfd> fds, int i)
{
	for (int j = 0; j < serverIndex.size(); j++)
	{
		if (serverIndex.at(j).first == fds[i].fd)
		{
			serverList.at(serverIndex.at(j).second).log(receivedData);
			break;
		}
	}

	int start = receivedData.find("data=") + 5;
	std::string wip = receivedData.substr(start);

	if (wip.compare("add") == 0)
		data.push_back(wip);
	if (wip.compare("remove") == 0)
	{
		if (data.size() > 0)
			data.pop_back();
	}
	if (wip.compare("print") == 0)
	{
		for (int i = 0; i < data.size(); i++)
		{
			std::cout << data.at(i).data() << std::endl;
		}
	}

	std::string response;
	std::string buffer;
	std::ifstream file("www/result.html");
	std::getline(file, buffer, '\0');
	std::stringstream headerStream;
	headerStream << "HTTP/1.1 200 OK\r\n"
				 << "Content-Length: " << buffer.size() << "\r\n"
				 << "\r\n";
	std::string header = headerStream.str();
	response.append(header);
	response.append(buffer);
	send(fds[i].fd, response.c_str(), response.length(), 0);
}

// DELETE
void Manager::handleDelete(std::string receivedData, std::vector<struct pollfd> fds, int i)
{
	for (int j = 0; j < serverIndex.size(); j++)
	{
		if (serverIndex.at(j).first == fds[i].fd)
		{
			serverList.at(serverIndex.at(j).second).log(receivedData);
			break;
		}
	}

	std::string response;
	int index;

	for (index = 0; index < serverIndex.size(); index++)
	{
		if (serverIndex.at(index).first == fds[i].fd)
			break;
	}

	std::string body = "OK";
	response = serverList.at(serverIndex.at(index).second).makeHeader(200, body.size());
	response.append(body);
	send(fds[i].fd, response.c_str(), response.length(), 0);
}

// OTHER
void Manager::handleOther(std::string receivedData, std::vector<struct pollfd> fds, int i)
{
	for (int j = 0; j < serverIndex.size(); j++)
	{
		if (serverIndex.at(j).first == fds[i].fd)
		{
			serverList.at(serverIndex.at(j).second).log(receivedData);
			break;
		}
	}

	std::string response;
	int index;

	for (index = 0; index < serverIndex.size(); index++)
	{
		if (serverIndex.at(index).first == fds[i].fd)
			break;
	}

	std::string body = "Method Not Implemented";
	response = serverList.at(serverIndex.at(index).second).makeHeader(501, body.size());
	response.append(body);
	send(fds[i].fd, response.c_str(), response.length(), 0);
}
