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

namespace LISTING_9_9 {

thread_local interrupt_flag this_thread_interrupt_flag;
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

	virtual ~interruptiable_thread();

	void interrupt();

};

} /* namespace LISTING_9_9 */

#endif /* INTERRUPTIABLETHREAD_H_ */
