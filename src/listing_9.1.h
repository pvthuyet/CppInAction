/*
 * listing_9.1.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_1_H_
#define LISTING_9_1_H_
#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include "listing_6.7.h"
#include "listing_8.3.h"
namespace LISTING_9_1 {
	class thread_pool {
	private:
		std::atomic_bool done;
		LISTING_6_7::threadsafe_queue<std::function<void()> > work_queue;
		std::vector<std::thread> threads;
		LISTING_8_3::join_threads joiner;

		void worker_thread() {
			while (!done) {
				std::function<void()> task;
				if(work_queue.try_pop(task)) {
					task();
				} else {
					std::this_thread::yield();
				}
			}
		}

	public:
		thread_pool() : done(false), joiner(threads) {
			unsigned const thread_count = std::thread::hardware_concurrency();
			try {
				for(unsigned i = 0; i < thread_count; ++i) {
					threads.emplace_back(std::thread(&thread_pool::worker_thread, this));
				}
			}
			catch(...) {
				done = true;
				throw;
			}
		}
		virtual ~thread_pool() {
			done = true;
		}

		template<typename FunctionType>
		void submit(FunctionType f) {
			work_queue.push(std::function<void()>(f));
		}
	};
	void test() {
		std::mutex gMutLog;
		std::atomic_int count(30);
		auto log = [&gMutLog, &count]() {
			for(;;) {
				if (count < 1)
					return;
				int var = --count;
				std::lock_guard<std::mutex> lock(gMutLog);
				std::cout << "Thread " << std::this_thread::get_id() << " timeout = " << var << std::endl << std::flush;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};
		thread_pool tPool;
		tPool.submit(log);
		tPool.submit(log);
		tPool.submit(log);
		tPool.submit(log);
		while(count > 1) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
}



#endif /* LISTING_9_1_H_ */
