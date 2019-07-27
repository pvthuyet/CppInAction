/*
 * listing_6.2.h
 *
 *  Created on: Jul 27, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_2_H_
#define LISTING_6_2_H_

#include <mutex>
#include <queue>
#include <condition_variable>
#include <thread>
#include <iostream>

namespace LISTING_6_2 {
	template<typename T>
	class threadsafe_queue {
	private:
		mutable std::mutex mut;
		std::queue<T> data_queue;
		std::condition_variable data_cond;

	public:
		threadsafe_queue() {}

		void push(T new_value) {
			std::lock_guard<std::mutex> lock(mut);
			data_queue.emplace(std::move(new_value));
			data_cond.notify_one();
		}

		void wait_and_pop(T& value) {
			std::unique_lock<std::mutex> lock(mut);
			data_cond.wait(lock, [this]{return !data_queue.empty();});
			value = std::move(data_queue.front());
			data_queue.pop();
		}

		std::shared_ptr<T> wait_and_pop() {
			std::unique_lock<std::mutex> lock(mut);
			data_cond.wait(lock, [this]{return !data_queue.empty();});
			std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
			data_queue.pop();
			return res;
		}

		bool try_pop(T& value) {
			std::lock_guard<std::mutex> lock(mut);
			if (data_queue.empty())
				return false;
			value = std::move(data_queue.front());
			data_queue.pop();
			return true;
		}

		std::shared_ptr<T> try_pop() {
			std::lock_guard<std::mutex> lock(mut);
			if (data_queue.empty())
				return std::make_shared<T>();
			std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
			data_queue.pop();
			return res;
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(mut);
			return data_queue.empty();
		}
	};


	std::mutex gMutLog;
	void test() {
		threadsafe_queue<int> queue;
		auto cb = [&queue](std::string act) {
			if (act == "push") {
				int n = 50;
				for(int i = 0; i < n; i++) {
					std::this_thread::sleep_for(std::chrono::seconds(1));
					queue.push(i);
					{
						std::lock_guard<std::mutex> lock(gMutLog);
						std::cout <<"Thread " << std::this_thread::get_id() << " Push " << i << " into queue\n" << std::flush;
					}
				}
			} else if (act == "pop") {
				int n = 10;
				for (int i=0; i < n; i++) {
					std::this_thread::sleep_for(std::chrono::seconds(3));
					auto value = queue.wait_and_pop();
					{
						std::lock_guard<std::mutex> lock(gMutLog);
						std::cout << "Thread " << std::this_thread::get_id() << " pop " << *value << " out queue\n"<< std::flush;
					}
				}
			}
		};

		std::thread push(cb, "push");
		std::thread pop1(cb, "pop");
		std::thread pop2(cb, "pop");
		std::thread pop3(cb, "pop");
		push.join();
		pop1.join();
		pop2.join();
		pop3.join();
	}
}



#endif /* LISTING_6_2_H_ */
