/*
 * listing_9.2.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_2_H_
#define LISTING_9_2_H_
#include "listing_6.7.h"
#include <future>
#include <functional>

namespace LISTING_9_2 {
	class function_wrapper {
	private:

	};

	class thread_pool {
	private:
		LISTING_6_7::threadsafe_queue<function_wrapper> work_queue;
		std::atomic_bool done;
		void worker_thread() {
			while(!done) {
				function_wrapper task;
				if(work_queue.try_pop(task)) {
					task();
				} else {
					std::this_thread::yield();
				}
			}
		}

	public:
		template<typename FunctionType>
		std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
			using result_type = typename std::result_of<FunctionType()>::type;
			std::packaged_task<result_type()> task(std::move(f));
			std::future<result_type> res(task.get_future());
			work_queue.push(std::move(task));
			return res;
		}
	};
}



#endif /* LISTING_9_2_H_ */
