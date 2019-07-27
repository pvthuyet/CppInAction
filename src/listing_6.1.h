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

namespace LISTING_6_1 {

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
}


#endif /* LISTING_6_1_H_ */
