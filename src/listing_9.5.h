/*
 * listing_9.5.h
 *	Please check quick sort at: https://en.cppreference.com/w/cpp/algorithm/partition
 *  Created on: Aug 11, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_5_H_
#define LISTING_9_5_H_
#include "listing_9.2.h"
#include <list>
#include <chrono>
#include <random>
#include <algorithm>
#include <iterator>
#include <functional>
#include <sstream>
#include "Utility.h"

extern tet::logger gLogger;

namespace LISTING_9_5 {
	template<typename T>
	struct sorter {
		LISTING_9_2::thread_pool pool;
		std::list<T> do_sort(std::list<T>& chunk_data) {
			std::ostringstream stringStream;
			stringStream << "Thread " << std::this_thread::get_id() << " sorting...: ";
			gLogger.debug(stringStream.str(), chunk_data);

			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (chunk_data.empty() || chunk_data.size() == 1) {
				return chunk_data;
			}

			std::list<T> result;
			result.splice(result.begin(), chunk_data, chunk_data.begin());
			T const& partition_val = *result.begin();
			auto divide_point = std::partition(chunk_data.begin(), chunk_data.end(),
					[&](const T& val){
				return val < partition_val;
			});
			// Processing Lower partition
			// move lower partition to new_lower_chunk
			std::list<T> new_lower_chunk;
			new_lower_chunk.splice(new_lower_chunk.end(), chunk_data, chunk_data.begin(), divide_point);
			// push to thread pool queue
			auto cb = std::bind(&sorter::do_sort, this, std::placeholders::_1);
			std::future<std::list<T> > new_lower = pool.submit(cb, new_lower_chunk);

			// processing higher partition
			std::list<T> new_higher(do_sort(chunk_data));
			result.splice(result.end(), new_higher);

			// wait for finish lower
			while(new_lower.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
				pool.run_pending_task();
			}

			result.splice(result.begin(), new_lower.get());
			return result;
		}
	};

	template<typename T>
	std::list<T> parallel_quick_sort(std::list<T> input) {
		if(input.empty() || input.size() == 1) {
			return input;
		}
		sorter<T> a;
		return a.do_sort(input);
	}

	void test() {
		char c = 'b';
		while (c != 'e') {
			std::cout << "Thread Pool - Choose running Algorithm (h for help): ";
			std::cin >> c;
			switch (c) {
			case 'e':
				break;
			case 'h':
				std::cout << std::endl;
				std::cout << "'e' - Exit" << std::endl;
				std::cout << "'a' - accumulate" << std::endl;
				std::cout << "'q' - quick sort" << std::endl;
				std::cout << "'h' - help" << std::endl;
				break;
			case 'q':
				{
					constexpr int N = 1'000;
					int sz;
					std::cout << std::endl;
					std::cout << "Enter number of elements (default " << N << " ): ";
					std::cin >> sz;
					if (sz < 2 ) {
						sz = N;
					}
					std::list<int> v = tet::Utility::random(sz, -100, 100);

					std::list<int> result  = parallel_quick_sort<int>(v);
					std::cout << "Quick sort result: ";
					std::copy(result.begin(), result.end(), std::ostream_iterator<int>(std::cout, " "));
					std::cout << "\n";
				}
			break;

			default:
				break;
			}
		}
	}
}



#endif /* LISTING_9_5_H_ */
