/*
 * listing_7.1.h
 *	Implementation of the spin-lock mutex using std::atomic_flag
 *  Created on: Aug 1, 2019
 *      Author: thuye
 */

#ifndef LISTING_7_1_H_
#define LISTING_7_1_H_

#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>

namespace LISTING_7_1 {
	class spinlock_mutex {
	private:
		std::atomic_flag flag;
	public:
		spinlock_mutex() : flag(ATOMIC_FLAG_INIT) {}
		void lock() {
			while(flag.test_and_set(std::memory_order_acquire));
		}
		void unlock() {
			flag.clear(std::memory_order_release);
		}
	};
	void test() {
		int var = 0;
		spinlock_mutex sm;
		std::mutex gMutLog;
		auto f = [&]() {
			for (int i = 0; i < 10; i++) {
				sm.lock();
				var++;
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << "\t" << var << std::endl << std::flush;
				}
				sm.unlock();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		};
		std::thread t1(f);
		std::thread t2(f);
		std::thread t3(f);
		t1.join();
		t2.join();
		t3.join();
	}
}



#endif /* LISTING_7_1_H_ */
