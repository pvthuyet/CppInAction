/*
 * log.cpp
 *
 *  Created on: Aug 10, 2019
 *      Author: thuye
 */

#include <iostream>
#include <thread>

#include "logger.h"
namespace tet {
	logger::logger() {
		// TODO Auto-generated constructor stub
	}

	logger::~logger() {
		// TODO Auto-generated destructor stub
	}

	void logger::debug(int a, int b) {
		std::lock_guard<std::mutex> lk(m_mut);
		std::cout << "Thread " << std::this_thread::get_id()<< ": ";
		int rs = 0;
		for (int i = a; i <= b; ++i) {
			rs = rs + i;
			std::cout << i;
			if (i != (b))
				std::cout << " + ";
		}
		std::cout << "=" << rs << std::endl << std::flush;
	}
}
