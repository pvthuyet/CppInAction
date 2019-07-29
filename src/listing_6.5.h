/*
 * listing_6.5.h
 * A simple queue with dummy node
 *  Created on: Jul 28, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_5_H_
#define LISTING_6_5_H_

#include <memory>

namespace LISTING_6_5 {
	template<typename T>
	class queue {
	private:
		struct node {
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
			node():data(nullptr), next(nullptr){}
		};
		std::unique_ptr<node> head;
		node* tail;

	public:
		queue():head(std::make_unique<node>()), tail(head.get()){}
		queue(const queue&) = delete;
		queue& operator=(const queue&) = delete;

		void push(T new_value) {
			auto new_data(std::make_shared<T>(std::move(new_value)));
			tail->data = new_data;

			auto p(std::make_unique<node>());
			node* const new_tail = p.get();
			tail->next = std::move(p);
			tail = new_tail;
		}

		std::shared_ptr<T> try_pop() {
			if (tail == head.get()) {
				return std::shared_ptr<T>();
			}
			auto const res(head->data);
			auto const old_head = std::move(head);
			head = std::move(old_head->next);
			return res;
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



#endif /* LISTING_6_5_H_ */
