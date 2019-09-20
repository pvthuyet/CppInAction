/*
 * interruptiablethread.h
 *
 *  Created on: Sep 19, 2019
 *      Author: thuye
 */

#ifndef INTERRUPTIABLETHREAD_H_
#define INTERRUPTIABLETHREAD_H_

#include "interruptflag.h"
#include <thread>
#include <future>
#include <condition_variable>
#include <chrono>

namespace LISTING_9_9 {

	thread_local interrupt_flag this_thread_interrupt_flag;
	struct clear_cv_on_destruct {
		~clear_cv_on_destruct() {
			this_thread_interrupt_flag.clear_condition_variable();
		}
	};

	class interruptiable_thread {
	private:
		std::thread internal_thread;
		interrupt_flag* flag;

	public:
		template<typename FunctionType>
		interruptiable_thread(FunctionType f) {
			std::promise<interrupt_flag*> p;
			internal_thread = std::thread([f, &p] {
				p.set_value(&this_thread_interrupt_flag);
				f();
			});
			flag = p.get_future().get();
		}

		virtual ~interruptiable_thread() {}

		void interrupt() {
			if (flag) {
				flag->set();
			}
		}

		void interruption_point() {
			if (this_thread_interrupt_flag.is_set()) {
				throw "Thread interrupted!";
			}
		}

		void interruptiable_wait(std::condition_variable& cv, std::unique_lock<std::mutex>& lk) {
			interruption_point();
			this_thread_interrupt_flag.set_condition_variable(cv);
			clear_cv_on_destruct guard;
			interruption_point();
			cv.wait_for(lk, std::chrono::milliseconds(1));
			interruption_point();
		}

		template<typename Predicate>
		void interruptible_wait(std::condition_variable& cv,
				std::unique_lock<std::mutex>& lk,
				Predicate pred) {
			interruption_point();
			this_thread_interrupt_flag.set_condition_variable(cv);
			clear_cv_on_destruct guard;
			while(!this_thread_interrupt_flag.is_set() && !pred()) {
				cv.wait_for(lk, std::chrono::milliseconds(1));
			}
			interruption_point();
		}
	};

} /* namespace LISTING_9_9 */

#endif /* INTERRUPTIABLETHREAD_H_ */
