#include "WebSocket.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

WebSocket::WebSocket(std::string ip, std::string port)
	:m_listenDesc(INVALID_SOCKET)
{
	// Winsock Stuff
	WSADATA wsaData;

	int result = 0;
	// Initialize Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) 
	{
		freeResources();
		throw std::runtime_error("Failed");
	}

	m_wsInit = true;

	// Init data for getting address of socket
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// Get address
	result = getaddrinfo(ip.c_str(), port.c_str(), &hints, &m_addrInfo);
	if (result != 0) 
	{
		freeResources();
		throw std::runtime_error("Failed");
	}

	// Get socket
	m_listenDesc = socket(m_addrInfo->ai_family, m_addrInfo->ai_socktype, m_addrInfo->ai_protocol);
	if (m_listenDesc == INVALID_SOCKET)
	{
		freeResources();
		throw std::runtime_error("Failed");
	}

	// Binding
	result = bind(m_listenDesc, m_addrInfo->ai_addr, (int)m_addrInfo->ai_addrlen);
	if (result == SOCKET_ERROR) 
	{
		freeResources();
		throw std::runtime_error("Failed");
	}

	result = listen(m_listenDesc, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		freeResources();
		throw std::runtime_error("Failed");
	}
}

WebSocket::~WebSocket()
{
	freeResources();
}

std::unique_ptr<ClientWebSocketHandle> WebSocket::waitForConnection()
{
	int clientSocket = accept(m_listenDesc, NULL, NULL);
	if (clientSocket == INVALID_SOCKET) 
		return nullptr;

	return std::unique_ptr<ClientWebSocketHandle>(new ClientWebSocketHandle(clientSocket));
}

void WebSocket::freeResources()
{
	if (m_addrInfo)
		freeaddrinfo(m_addrInfo);
	if (m_listenDesc != INVALID_SOCKET)
		closesocket(m_listenDesc);
	if(m_wsInit)
		WSACleanup();
}

bool ClientWebSocketHandle::sendResponse(std::string data)
{
	int result = send(m_clientDesc, data.c_str(), data.length(), 0);
	return result != SOCKET_ERROR;
}

std::string ClientWebSocketHandle::recieveRequest()
{
	const int maxSize = 1024;
	char buf[maxSize];

	int result = recv(m_clientDesc, buf, maxSize, 0);

	if (result <= 0)
		return std::string();

	buf[result] = '\0';
	return std::string(buf);	
}

ClientWebSocketHandle::~ClientWebSocketHandle()
{
	closesocket(m_clientDesc);
}