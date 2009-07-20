#ifndef TQUEUE_H
#define TQUEUE_H

#include <queue>
#include <boost/thread.hpp>
using namespace boost;

template <typename T>
class tqueue {
public:
	tqueue(){}
	virtual ~tqueue(){}

	void enqueue(T t) {
		{
			unique_lock<boost::mutex> lock(mut);
			q.push(t);
		}
		wait.notify_one();
	}

	T dequeue() {
		unique_lock<boost::mutex> lock(mut);
		while (q.empty()) {
			wait.wait(lock);
		}
		T front = q.front();
		q.pop();
		return front;
	}

private:
	std::queue<T> q;
	boost::mutex mut;
	boost::condition_variable wait;
};

#endif // TQUEUE_H
