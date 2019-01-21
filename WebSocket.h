#pragma once
#include <string>
#include <memory>

struct addrinfo;

class ClientWebSocketHandle;

class WebSocket
{
public:
	WebSocket(std::string ip, std::string port);
	~WebSocket();

	std::unique_ptr<ClientWebSocketHandle> waitForConnection();

private:
	void  freeResources();
private:
	using Descriptor = int;

	bool m_wsInit = false;
	struct addrinfo * m_addrInfo = nullptr;
	Descriptor m_listenDesc;
};

class ClientWebSocketHandle
{
	friend class WebSocket;
public:
	bool sendResponse(std::string data);
	std::string recieveRequest();
	~ClientWebSocketHandle();
private:
	using Descriptor = int;
	ClientWebSocketHandle(Descriptor desc) : m_clientDesc(desc){}
	Descriptor m_clientDesc;
};