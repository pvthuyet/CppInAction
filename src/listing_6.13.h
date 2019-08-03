/*
 * listing_6.13.h
 *
 *  Created on: Aug 3, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_13_H_
#define LISTING_6_13_H_

#include <mutex>
#include <memory>
#include <thread>
#include <chrono>

namespace LISTING_6_13 {
	template<typename T>
	class threadsafe_list {
	private:
		struct node {
			std::mutex m;
			std::shared_ptr<T> data;
			std::unique_ptr<node> next;
			node() = default;
			node(T const& new_val):data(std::make_shared<T>(new_val)) {}
		};
		node head;

	public:
		threadsafe_list() = default;
		threadsafe_list(threadsafe_list const &) = delete;
		threadsafe_list& operator=(threadsafe_list const&) = delete;
		virtual ~threadsafe_list() {
			remove_if([](node const* elm) {
				return true;
			});
		}

		template<typename Predicate>
		void remove_if(Predicate p) {
			node* current = &head;
			std::unique_lock<std::mutex> lk(head.m);
			while(node* const next = current->next.get()) {
				std::unique_lock<std::mutex> next_lk(next->m);
				if (p(next)) {
					auto old_next = std::move(current->next);
					current->next = std::move(next->next);
					next_lk.unlock();
				} else {
					lk.unlock();
					current = next;
					lk = std::move(next_lk);
				}
			}
		}

		void push_front(T const& new_val) {
			auto new_node(std::make_unique<node>(new_val));
			std::lock_guard<std::mutex> lock(head.m);
			new_node->next = std::move(head.next);
			head.next = std::move(new_node);
		}

		template<typename Function>
		void for_each(Function f) {
			node* current = &head;
			std::unique_lock<std::mutex> lk(current->m);
			while(node* const next = current->next.get()) {
				std::unique_lock<std::mutex> lk_next(next->m);
				lk.unlock();
				f(*next->data);
				current = next;
				lk = std::move(lk_next);
			}
		}

		template<typename Predicate>
		std::shared_ptr<T> find_first_if(Predicate p) {
			node* current = &head;
			std::unique_lock<std::mutex> lk(current->m);
			while(node* const next = current->next.get()) {
				std::unique_lock<std::mutex> lk_next(next->m);
				lk.unlock();
				if (p(*next->data)) {
					return next->data;
				}
				current = next;
				lk = std::move(lk_next);
			}
			return nullptr;
		}
	};

	void test() {
		std::mutex gMutLog;
		threadsafe_list<int> list;
		auto pf = [&list, &gMutLog](int beg) {
			constexpr int N = 10;
			for (int i = beg; i < N + beg; ++i) {
				list.push_front(i);
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " add\t" << i << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};

		auto findf = [&list, &gMutLog](int exp) {
			auto find_f = [exp](int v) {
				return (v == exp ? true : false);
			};
			while(true) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				auto found = list.find_first_if(find_f);
				if (found) {
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " found value " << *found << std::endl << std::flush;
					return;
				} else {
					std::cout << "Thread " << std::this_thread::get_id() << " continue finding... " << std::endl << std::flush;
				}
			}
		};
		std::thread t1(pf, 0);
		std::thread t2(pf, 10);
		std::thread t3(findf, 20);

		t1.join();
		t2.join();
		t3.join();
	}
}



#endif /* LISTING_6_13_H_ */

