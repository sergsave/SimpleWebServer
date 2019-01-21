#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue
{
public:

	T pop()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_deque.empty())
			m_condDataAvailable.wait(lock);

		auto elem = m_deque.front();
		m_deque.pop_front();
		return elem;
	};

	void push(const T& t)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_deque.push_back(t);
		}
		m_condDataAvailable.notify_one();
	}

private:
	std::deque<T> m_deque;
	std::mutex m_mutex;
	std::condition_variable m_condDataAvailable;
};
