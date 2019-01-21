#pragma once

#include "WebSocket.h"
#include <memory>

// Greet the client.
class WebWorker
{
public:
	WebWorker(std::unique_ptr<ClientWebSocketHandle>& handle);

	void execute();
private:
	std::shared_ptr<ClientWebSocketHandle> m_socketHandle;
};