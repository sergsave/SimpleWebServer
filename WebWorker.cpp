#include "WebWorker.h"
#include <sstream>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>

static std::mutex g_mutex;

WebWorker::WebWorker(std::unique_ptr<ClientWebSocketHandle>& handle):
	m_socketHandle(std::move(handle))
{}

void WebWorker::execute()
{
	std::lock_guard<std::mutex> guard(g_mutex);
	auto req = m_socketHandle->recieveRequest();

	if (req.empty())
		return;

	auto printTime = []() 
	{
		auto sourceT = std::time(nullptr);
		auto time = std::asctime(std::localtime(&sourceT));
		std::string timeStr(time);
		// OMG!!!
		timeStr = timeStr.substr(12, 7);

		std::cout << timeStr << " ";
	};

	printTime();
	std::cout << "Request received" << std::endl;

	// For test.
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	std::stringstream response;
	std::string message("Hello, i'm WWWWWEB SERVER!");

	response 
		<< "HTTP/1.1 200 OK\r\n"
		<< "Version: HTTP/1.1\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " 
		<< message.length()
		<< "\r\n\r\n"
		<< message;

	m_socketHandle->sendResponse(response.str());

	printTime();
	std::cout << "Response sended" << std::endl;
}
