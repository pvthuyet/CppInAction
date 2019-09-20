/*
 * interruptflag.cpp
 *
 *  Created on: Sep 19, 2019
 *      Author: thuye
 */

#include "interruptflag.h"

namespace LISTING_9_9 {

interrupt_flag::interrupt_flag() : thread_cond(0) {
	// TODO Auto-generated constructor stub

}

interrupt_flag::~interrupt_flag() {
	// TODO Auto-generated destructor stub
}

void interrupt_flag::set() {
	flag.store(true, std::memory_order_relaxed);
	std::lock_guard<std::mutex> lk(set_clear_mutex);
	if (thread_cond) {
		thread_cond->notify_all();
	}
}

bool interrupt_flag::is_set() const {
	return flag.load(std::memory_order_relaxed);
}

void interrupt_flag::set_condition_variable(std::condition_variable& cv) {
	std::lock_guard<std::mutex> lk(set_clear_mutex);
	thread_cond = &cv;
}

void interrupt_flag::clear_condition_variable() {
	std::lock_guard<std::mutex> lk(set_clear_mutex);
	thread_cond = 0;
}
} /* namespace LISTING_9_9 */
