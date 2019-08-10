/*
 * listing_9.1.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_2_H_
#define LISTING_9_2_H_
#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>
#include "listing_6.7.h"
#include "listing_8.3.h"
#include <system_error>
namespace LISTING_9_2 {
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
		thread_pool(unsigned numThread = std::thread::hardware_concurrency()) : done(false), joiner(threads) {
			try {
				for(unsigned i = 0; i < numThread; ++i) {
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

		template<class Fn, class... Args>
		auto submit(Fn&& f, Args&&... args)
			-> std::future<std::result_of_t<Fn(Args...)> >
		{
			using return_type = std::result_of_t<Fn(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
			(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
			);
			std::future<return_type> res = task->get_future();

			work_queue.push([task](){
				(*task)();
			});
			return res;
		}
	};
	void test() {
		std::mutex gMutLog;
		std::atomic_int count(30);
		auto log = [&gMutLog, &count](int a) -> int {
			for(;;) {
				if (count < 1)
					return a;
				count += a;
				int cur = count;
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " plus " << a << " timeout = " << cur << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			return a;
		};
		thread_pool tPool;
		auto f1 = tPool.submit(log, 1);
		auto f2 = tPool.submit(log, -2);
		auto f3= tPool.submit(log, 3);
		auto f4 = tPool.submit(log, -4);
		auto r1 = f1.get();
		auto r2 = f2.get();
		auto r3 = f3.get();
		auto r4 = f4.get();
		std::cout << "f1: " << r1 << std::endl << std::flush;
		std::cout << "f2: " << r2 << std::endl << std::flush;
		std::cout << "f3: " << r3 << std::endl << std::flush;
		std::cout << "f4: " << r4 << std::endl << std::flush;
	}
}



#endif /* LISTING_9_2_H_ */
