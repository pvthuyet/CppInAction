/*
 * log.cpp
 *
 *  Created on: Aug 10, 2019
 *      Author: thuye
 */

#include <iostream>
#include <thread>

#include "logger.h"
#include <iterator>
#include <functional>
#include <algorithm>

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
	void logger::debug(std::string str) {
		std::lock_guard<std::mutex> lk(m_mut);
		std::cout << str;
	}

	void logger::debug(std::string str, std::list<int> input) {
		std::lock_guard<std::mutex> lk(m_mut);
		std::cout << str;
		std::copy(input.begin(), input.end(), std::ostream_iterator<int>(std::cout, " "));
		std::cout << std::endl;
	}
}
