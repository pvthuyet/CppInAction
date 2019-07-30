/*
 * listing_6.6.h
 * Thread-safe queue with fine-grained locking
 *  Created on: Jul 29, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_6_H_
#define LISTING_6_6_H_

#include <memory>
#include <mutex>
#include <condition_variable>

namespace LISTING_6_6 {
	template<typename T>
	class threadsafe_queue {
	private:
		struct node {
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
		};
		std::mutex head_mutex;
		std::unique_ptr<node> head;

		std::mutex tail_mutex;
		node* tail;

		node* get_tail(){
			std::lock_guard<std::mutex> lock(tail_mutex);
			return tail;
		}

		std::unique_ptr<threadsafe_queue::node> pop_head() {
			std::lock_guard<std::mutex> lock(head_mutex);
			if (get_tail() == head.get()) {
				return nullptr;
			}
			auto old_head = std::move(head);
			head = std::move(old_head->next);
			return old_head;
		}

	public:
		threadsafe_queue();
		void push(T new_value);
		std::shared_ptr<T> try_pop();
		bool try_pop(T value);
	};

	// Create a queue with a dummy node.
	template<typename T>
	threadsafe_queue<T>::threadsafe_queue()
	: head(std::make_unique<node>()), tail(head.get()) {}

	template<typename T>
	void threadsafe_queue<T>::push(T new_value) {
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		std::unique_ptr<node> p(std::make_unique<node>());
		node* new_tail = p.get();
		std::lock_guard<std::mutex> lock(tail_mutex);
		tail->data = new_data;
		tail->next = std::move(p);
		tail = new_tail;
	}

	template<typename T>
	std::shared_ptr<T> threadsafe_queue<T>::try_pop() {
		auto old_head = pop_head();
		return old_head? old_head->data : nullptr;
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
				auto v = que.try_pop();
				if (v) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " pop " << *v << " out queue" << std::endl << std::flush;
				} else {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " empty queue!" << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};
		std::thread push1(pushF, 0, 10);
		std::thread push2(pushF, 10, 10);
		std::thread push3(pushF, 20, 10);
		std::thread pop1(popF);
		std::thread pop2(popF);
		push1.join();
		push2.join();
		push3.join();
		pop1.join();
		pop2.join();
	}
}



#endif /* LISTING_6_6_H_ */
