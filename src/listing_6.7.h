/*
 * listing_6.7.h
 *	A thread-safe queue with locking and waiting: internal and interface
 *	this includes listing 6.7, listing 6.8, listing 6.9, listing 6.10
 *  Created on: Jul 29, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_7_H_
#define LISTING_6_7_H_

#include <memory>
#include <condition_variable>
#include <mutex>

namespace LISTING_6_7 {
	template<typename T>
	class threadsafe_queue {
	private:
		struct node {
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
		};
		std::condition_variable cond;
		std::mutex head_mutex;
		std::unique_ptr<node> head;

		std::mutex tail_mutex;
		node* tail;

		// APIs
		threadsafe_queue<T>::node* get_tail() {
			std::lock_guard<std::mutex> lock(tail_mutex);
			return tail;
		}

		std::unique_ptr<node> pop_head() {
			std::unique_ptr<node> old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

		std::unique_lock<std::mutex> wait_for_data() {
			std::unique_lock<std::mutex> lock(head_mutex);
			cond.wait(lock, [&]{ return head.get() != get_tail(); });
			return std::move(lock);
		}

		std::unique_ptr<node> try_pop_head() {
			std::lock_guard<std::mutex> lock(head_mutex);
			if (head.get() == get_tail()) {
				return nullptr;
			}
			return pop_head();
		}

		std::unique_ptr<node> try_pop_head(T& value) {
			std::lock_guard<std::mutex> lock(head_mutex);
			if (head.get() == get_tail()) {
				return nullptr;
			}
			value = std::move(*head->data);
			return pop_head();
		}

		std::unique_ptr<node> wait_pop_head() {
			std::unique_lock<std::mutex> lock(wait_for_data());
			return pop_head();
		}

		std::unique_ptr<node> wait_pop_head(T& value) {
			std::unique_lock<std::mutex> lock(wait_for_data());
			value = std::move(*head->data);
			return pop_head();
		}

	public:
		threadsafe_queue();
		threadsafe_queue(const threadsafe_queue&) = delete;
		threadsafe_queue& operator=(const threadsafe_queue&) = delete;

		bool empty();
		void push(T new_value);

		std::shared_ptr<T> try_pop();
		bool try_pop(T& value);

		std::shared_ptr<T> wait_and_pop();
		void wait_and_pop(T& value);
	};

	template<typename T>
	threadsafe_queue<T>::threadsafe_queue():
			head(std::make_unique<node>()), tail(head.get()){}

	template<typename T>
	bool threadsafe_queue<T>::empty() {
		std::lock_guard<std::mutex> lock(head_mutex);
		return (head.get() == get_tail());
	}

	template<typename T>
	void threadsafe_queue<T>::push(T new_value) {
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		std::unique_ptr<node> p(std::make_unique<node>());
		node* const new_tail = p.get();
		{
			std::lock_guard<std::mutex> lock(tail_mutex);
			tail->data = new_data;
			tail->next = std::move(p);
			tail = new_tail;
		}
		cond.notify_one();
	}

	template<typename T>
	std::shared_ptr<T> threadsafe_queue<T>::try_pop() {
		auto old_head = try_pop_head();
		return old_head ? old_head->data : nullptr;
	}

	template<typename T>
	bool threadsafe_queue<T>::try_pop(T& value) {
		auto old_head = try_pop_head(value);
		return old_head ? true : false;
	}

	template<typename T>
	std::shared_ptr<T> threadsafe_queue<T>::wait_and_pop() {
		std::unique_ptr<node> const old_head = wait_pop_head();
		return old_head->data;
	}

	template<typename T>
	void threadsafe_queue<T>::wait_and_pop(T& value) {
		std::unique_ptr<node> const old_head = wait_pop_head(value);
	}

	void test() {
		std::mutex gMutLog;
		threadsafe_queue<int> que;
		auto pushF = [&que, &gMutLog](int s, int N){
			std::this_thread::sleep_for(std::chrono::seconds(1));
			for (int i=s; i < s+N; i++) {
				que.push(i);
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " push " << i << " into queue" << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

		};

		auto popF = [&que, &gMutLog](){
			std::this_thread::sleep_for(std::chrono::seconds(1));
			for(;;) {
				//auto v = que.wait_and_pop();
				int v = -1;
				que.wait_and_pop(v);
				if (v != -1) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " pop " << v << " out queue" << std::endl << std::flush;
				} else {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " empty queue!" << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};
		std::thread push1(pushF, 0, 10);
		std::thread push2(pushF, 10, 10);
		//std::thread push3(pushF, 20, 10);
		std::thread pop1(popF);
		//std::thread pop2(popF);
		push1.join();
		push2.join();
		//push3.join();
		pop1.join();
		//pop2.join();
	}
}



#endif /* LISTING_6_7_H_ */
