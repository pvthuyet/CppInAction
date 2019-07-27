/*
 * listing_6.2.h
 * A class definition for a thread-safe stack
 *  Created on: Jul 27, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_1_H_
#define LISTING_6_1_H_

#include <exception>
#include <stack>
#include <mutex>
#include <memory>
#include <future>
#include <chrono>
#include <thread>

namespace LISTING_6_1 {
	std::mutex gMutLog;

	struct empty_stack : std::exception {
		const char* what() const throw() { return "Empty stack"; }
	};

	template<typename T>
	class threadsafe_stack {
	private:
		std::stack<T> data;
		mutable std::mutex m;

	public:
		threadsafe_stack() {}
		threadsafe_stack(const threadsafe_stack& other) {
			std::lock_guard<std::mutex> lock(other.m);
			data = other.data;
		}

		threadsafe_stack& operator=(const threadsafe_stack&) = delete;

		void push(T new_value) {
			std::lock_guard<std::mutex> lock(m);
			data.push(std::move(new_value));
		}

		std::shared_ptr<T> pop() {
			std::lock_guard<std::mutex> lock(m);
			if (data.empty()) throw empty_stack();
			std::shared_ptr<T> const res(std::make_shared<T>(std::move(data.top())));
			data.pop();
			return res;
		}

		void pop(T& value) {
			std::lock_guard<std::mutex> lock(m);
			if (data.empty()) throw empty_stack();
			value = std::move(data.top());
			data.pop();
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(m);
			return data.empty();
		}

	};

	void test () {
		LISTING_6_1::threadsafe_stack<int> stk;
		auto pPush = std::async(std::launch::async, [&stk](){
			const int n = 50;
			for (int i = 0; i < n; i++) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				stk.push(i);
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << std::this_thread::get_id() << " Push " << i << " into stack\n" << std::flush;
				}
			}
		});

		auto pPop1 = std::async(std::launch::async, [&stk](){
			const int n = 10;
			for (int i = 0; i < n; i++) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				try {
					auto value = stk.pop();
					{
						std::lock_guard<std::mutex> lock(gMutLog);
						std::cout << std::this_thread::get_id() << " pop " << *value << " out stack\n"<< std::flush;
					}
				} catch (const std::exception& e) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << std::this_thread::get_id() << " " << e.what() << std::endl << std::flush;
				}
			}
		});

		auto pPop2 = std::async(std::launch::async, [&stk](){
			const int n = 10;
			for (int i = 0; i < n; i++) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				try {
					auto value = stk.pop();
					{
						std::lock_guard<std::mutex> lock(gMutLog);
						std::cout << std::this_thread::get_id() << " pop " << *value << " out stack\n"<< std::flush;
					}
				} catch (const std::exception& e) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << std::this_thread::get_id() << " " << e.what() << std::endl<< std::flush;
				}
			}
		});

		auto pPop3 = std::async(std::launch::async, [&stk](){
			const int n = 10;
			for (int i = 0; i < n; i++) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				try {
					auto value = stk.pop();
					{
						std::lock_guard<std::mutex> lock(gMutLog);
						std::cout << std::this_thread::get_id() << " pop " << *value << " out stack\n"<< std::flush;
					}
				} catch (const std::exception& e) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << std::this_thread::get_id() << " " << e.what() << std::endl<< std::flush;
				}
			}
		});

		pPush.wait();
		pPop1.wait();
		pPop2.wait();
		pPop3.wait();

	}
}


#endif /* LISTING_6_1_H_ */
