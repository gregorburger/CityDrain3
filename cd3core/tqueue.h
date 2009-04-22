#ifndef TQUEUE_H
#define TQUEUE_H

#include <queue>
#include <QMutex>
#include <QWaitCondition>

template <typename T>
class tqueue
{
public:
	tqueue(){}

	void enqueue(T t) {
		lock.lock();
		q.push(t);
		lock.unlock();
		wait.wakeOne();
	}

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

private:
	std::queue<T> q;
	QMutex lock;
	QWaitCondition wait;
};

#endif // TQUEUE_H
