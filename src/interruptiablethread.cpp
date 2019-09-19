/*
 * interruptiablethread.cpp
 *
 *  Created on: Sep 19, 2019
 *      Author: thuye
 */

#include "interruptiablethread.h"

namespace LISTING_9_9 {

interruptiable_thread::~interruptiable_thread() {
	// TODO Auto-generated destructor stub
}

void interruptiable_thread::interrupt() {
	if (flag) {
		flag->set();
	}
}
} /* namespace LISTING_9_9 */
