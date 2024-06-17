#include "../incl/server.hpp"

// Generate status message for 2xx HTTP status codes
std::string Server::makeStatus2xx(int status)
{
	if (status == 200)
		return " OK";

	if (status == 201)
		return " Created";

	if (status == 202)
		return " Accepted";

	if (status == 203)
		return " Non-Authoritative Information";

	return " ERROR";
}

// Generate status message for 3xx HTTP status codes
std::string Server::makeStatus3xx(int status)
{
	if (status == 300)
		return " Multiple Choices";

	return " ERROR";
}

// Generate status message for 4xx HTTP status codes
std::string Server::makeStatus4xx(int status)
{
	if (status == 400)
		return " Bad Request";

	if (status == 404)
		return " Not Found";

	if (status == 413)
		return " Request Entity Too Large";

	if (status == 418)
		return " I'm a teapot";

	return " ERROR";
}

// Generate status message for 5xx HTTP status codes
std::string Server::makeStatus5xx(int status)
{
	if (status == 500)
		return " Internal Server Error";

	if (status == 501)
		return " Method Not Implemented";

	return " ERROR";
}

// Generate HTTP header
std::string Server::makeHeader(int responseStatus, int responseSize)
{
	std::stringstream headerStream;
	std::string header;

	headerStream << "HTTP/1.1 " << responseStatus;
	if (responseStatus >= 100 && responseStatus <= 199)
		headerStream << " ";
	else if (responseStatus >= 200 && responseStatus <= 299)
		headerStream << makeStatus2xx(responseStatus) << "\r\n";
	else if (responseStatus >= 300 && responseStatus <= 399)
		headerStream << makeStatus3xx(responseStatus) << "\r\n";
	else if (responseStatus >= 400 && responseStatus <= 499)
		headerStream << makeStatus4xx(responseStatus) << "\r\n";
	else if (responseStatus >= 500 && responseStatus <= 599)
		headerStream << makeStatus5xx(responseStatus) << "\r\n";

	headerStream << "Content-Length: " << responseSize << "\r\n\r\n";
	header = headerStream.str();
	if (header.find("ERROR") != std::string::npos)
	{
		return "ERROR";
	}
	return header;
}

// Build HTTP response
std::string Server::buildHTTPResponse(std::string fileName, std::string fileExt)
{
	std::string response;
	std::string mimeType = getMIMEType(fileExt);
	std::string header;
	std::string buffer;
	std::stringstream headerStream;
	std::stringstream responseStream;

	// If empty, i.e., front page
	if (fileName.empty())
	{
		std::string fileFull;
		fileFull.append(rootDir);
		fileFull.append("home.html");
		int fileFd = open(fileFull.data(), O_RDONLY);
		std::ifstream file(fileFull);
		if (file.is_open() == 0)
		{
			responseStream << "HTTP/1.1 200 OK\r\nContent-Length: 10\r\n\r\nFront page";
			response = responseStream.str();
			return response;
		}
		std::getline(file, buffer, '\0');
		header = makeHeader(200, buffer.size());
		response.append(header);
		response.append(buffer);
		return response;
	}

	// If some other page
	std::string fileFull;
	fileFull.append(rootDir);
	fileFull.append(fileName);
	fileFull.append(fileExt);
	int fileFd = open(fileFull.data(), O_RDONLY);
	std::ifstream file(fileFull);
	if (file.is_open() == 0)
	{
		std::string errorDir;
		errorDir.append(rootDir);
		errorDir.append(error404Dir);
		std::ifstream errormsg(errorDir);
		if (errormsg.is_open() == 0)
		{
			std::string body = "Page you were looking for does not exist, nor should it ever exist";
			response = makeHeader(404, body.size());
			response.append(body);
			return response;
		}
		std::getline(errormsg, buffer, '\0');
		header = makeHeader(404, buffer.size());
		response.append(header);
		response.append(buffer);
		return response;
	}
	std::getline(file, buffer, '\0');
	header = makeHeader(200, buffer.size());
	response.append(header);
	response.append(buffer);
	return response;
}
