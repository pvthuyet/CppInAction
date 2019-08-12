/*
 * listing_9.7.h
 *
 *  Created on: Aug 12, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_7_H_
#define LISTING_9_7_H_
#include "include.h"
namespace LISTING_9_7 {
	class work_stealing_queue {
	private:
		using data_type = std::function<void()>;
		std::deque<data_type> the_queue;
		mutable std::mutex the_mutex;

	public:
		work_stealing_queue() {}
		work_stealing_queue(const work_stealing_queue&) = delete;
		work_stealing_queue& operator=(work_stealing_queue const&) = delete;

		void push(data_type data) {
			std::lock_guard<std::mutex> lk(the_mutex);
			the_queue.push_front(std::move(data));
		}

		bool empty() {
			std::lock_guard<std::mutex> lk(the_mutex);
			return the_queue.empty();
		}

		bool try_pop(data_type& res) {
			std::lock_guard<std::mutex> lk(the_mutex);
			if (the_queue.empty()) {
				return false;
			}
			res = std::move(the_queue.front());
			the_queue.pop_front();
			return true;
		}

		bool try_steal(data_type& res) {
			std::lock_guard<std::mutex> lk(the_mutex);
			if (the_queue.empty()) {
				return false;
			}
			res = std::move(the_queue.back());
			the_queue.pop_back();
			return true;
		}
	};
}



#endif /* LISTING_9_7_H_ */
