#ifndef TQUEUE_H
#define TQUEUE_H

#define CD3_BOOST_SYNC

#include <queue>
#ifdef CD3_BOOST_SYNC
#include <boost/thread.hpp>
using namespace boost;
#else
#include <QMutex>
#include <QWaitCondition>
#endif

template <typename T>
class tqueue
{
public:
	tqueue(){}

#ifdef CD3_BOOST_SYNC
	void enqueue(T t) {
		{
			unique_lock<boost::mutex> lock(mut);
			q.push(t);
		}
		wait.notify_one();
	}
#else
	void enqueue(T t) {
		lock.lock();
		q.push(t);
		lock.unlock();
		wait.wakeOne();

	}
#endif

#ifdef CD3_BOOST_SYNC
	T dequeue() {
		unique_lock<boost::mutex> lock(mut);
		while (q.empty()) {
			wait.wait(lock);
		}
		T front = q.front();
		q.pop();
		return front;
	}
#else
	T dequeue() {
		lock.lock();
		while (q.empty()) {
			wait.wait(&lock);
		}
		T front = q.front();
		q.pop();
		lock.unlock();
		return front;
	}
#endif
private:
	std::queue<T> q;
#ifdef CD3_BOOST_SYNC
	boost::mutex mut;
	boost::condition_variable wait;
#else
	QMutex lock;
	QWaitCondition wait;
#endif
};

#endif // TQUEUE_H
