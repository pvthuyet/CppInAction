/*
 * listing_9.2.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_3_H_
#define LISTING_9_3_H_
#include "listing_6.7.h"
#include <future>
#include <functional>
#include <memory>
#include <iterator>
#include <numeric>
#include <vector>
#include <type_traits>
#include <algorithm>
#include "listing_9.2.h"
#include "logger.h"

extern tet::logger gLogger;
namespace LISTING_9_3 {
	template<typename Iterator, typename T>
	struct accumulate_block {
		T operator()(Iterator first, Iterator last) {
			gLogger.debug(*first, *(last-1));
			std::this_thread::sleep_for(std::chrono::seconds(1));
			return std::accumulate(first, last, T());
		}
	};

	template<typename Iterator, typename T>
	T parallel_accumulate(Iterator first, Iterator last, T init) {
		unsigned long const length = std::distance(first, last);
		if (!length)
			return init;
		unsigned long const block_size = 25;
		unsigned long const num_threads = 16;
		unsigned long const num_blocks = (length + block_size -1) / (block_size );
		std::vector<std::future<T> > futures(num_blocks - 1);
		LISTING_9_2::thread_pool pool(num_threads);

		Iterator block_start = first;
		for(unsigned long i = 0; i < (num_blocks - 1); ++i) {
			//T val;
			Iterator block_end = block_start;
			std::advance(block_end, block_size);
			futures[i] = pool.submit(accumulate_block<Iterator, T>(), block_start, block_end);
			block_start = block_end;
		}
		T last_result = accumulate_block<Iterator, T>()(block_start, last);
		T result = init;
		for(unsigned long i = 0; i < (num_blocks-1);++i) {
			result += futures[i].get();
		}
		result += last_result;
		return result;
	}
	void test() {
		constexpr int N = 10'000;
		std::vector<int> v(N);
		std::iota(std::begin(v), std::end(v), 1);

		int init = 0;
		int result = parallel_accumulate<std::vector<int>::iterator, int>(std::begin(v), std::end(v), init);
		std::cout << "parallel accumulate: "
				<< v[0] << " + "
				<< v[1] << " + ... + "
				<< v[N-1] << " = "
				<< result << std::endl << std::flush;
	}
}



#endif /* LISTING_9_3_H_ */
