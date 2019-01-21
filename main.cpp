#include <thread>
#include <vector>
#include <memory>
#include <stdexcept>
#include "SafeDeque.h"
#include "WebSocket.h"
#include "WebWorker.h"

int main()
{
	SafeQueue<WebWorker> workers;
	auto manageWorkers = [&workers]()
	{
		while (true)
		{
			auto w = workers.pop();
			w.execute();
		}
	};

	using std::thread;
	std::vector<thread> threads(thread::hardware_concurrency());
	for (auto & th : threads)
		th = thread(manageWorkers);

	std::unique_ptr<WebSocket> socket = nullptr;
	try
	{
		socket = std::make_unique<WebSocket>("127.0.0.1", "8000");
	}
	catch (const std::runtime_error&)
	{
		return 1;
	}

	while (true)
	{
		auto socketHandle = socket->waitForConnection();

		if (socketHandle)
			workers.push(WebWorker(socketHandle));
	}
}
