/**
 * CityDrain3 is an open source software for modelling and simulating integrated 
 * urban drainage systems.
 * 
 * Copyright (C) 2012 Gregor Burger
 * 
 * This program is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU General Public License as published by the Free Software 
 * Foundation; version 2 of the License.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this program; if not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 **/

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
			boost::unique_lock<boost::mutex> lock(mut);
			q.push(t);
		}
		wait.notify_one();
	}

	T dequeue() {
		boost::unique_lock<boost::mutex> lock(mut);
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
