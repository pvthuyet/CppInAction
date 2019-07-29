/*
 * listing_6.4.h
 * A simple single-threaded queue implementation
 *  Created on: Jul 28, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_4_H_
#define LISTING_6_4_H_

#include <memory>

namespace LISTING_6_4 {
	template<typename T>
	class queue {
	private:
		struct node {
			T data;
			std::unique_ptr<node> next;
			node(T data_) : data(std::move(data_)){}
		};
		std::unique_ptr<node> head;
		node* tail;

	public:
		queue(): tail(nullptr) {}
		queue(const queue& other) = delete;
		queue& operator=(const queue& other) = delete;

		std::shared_ptr<T> try_pop() {
			if (!head) {
				return std::shared_ptr<T>();
			}

			std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
			std::unique_ptr<node> const old_head = std::move(head);
			head = std::move(old_head->next);
			return res;
		}

		void push(T new_value) {
			std::unique_ptr<node> p(std::make_unique<node>(new_value));
			node* const new_tail = p.get();
			if (tail) {
				tail->next = std::move(p);
			} else {
				head = std::move(p);
			}

			tail = new_tail;
		}
	};

	void test() {
		queue<int> que;
		constexpr int N = 10;
		for(int i = 0; i < N; i++) {
			que.push(i);
			std::cout << "push " << i << " into queue" << std::endl << std::flush;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		for(;;) {
			auto v = que.try_pop();
			if (!v) {
				break;
			}
			std::cout << "pop " << *v << " out of queue" << std::endl << std::flush;
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}



#endif /* LISTING_6_4_H_ */
