/*
 * listing_9.2.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_2_H_
#define LISTING_9_2_H_
#include "listing_6.7.h"
#include <future>
#include <functional>
#include <memory>
#include <iterator>
#include <numeric>
#include <vector>
#include <type_traits>

namespace LISTING_9_2 {
	class function_wrapper {
	private:
		struct impl_base {
			virtual void call() = 0;
			virtual ~impl_base() {}
		};
		template<typename F>
		struct impl_type: impl_base {
			F f;
			impl_type(F&& f_): f(std::move(f_)) {}
			impl_type(F& f_) = delete;
			void call() { f(); }
		};
		std::unique_ptr<impl_base> impl;

	public:
		template<typename F>
		function_wrapper(F&& f) : impl(std::make_unique<impl_type<F> >(std::move(f)))
		{}
		void operator()() { impl->call(); }
		function_wrapper() = default;
		function_wrapper(function_wrapper&& other): impl(std::move(other.impl))
		{}
		function_wrapper& operator=(function_wrapper&& other) {
			impl = std::move(other.impl);
			return *this;
		}
		function_wrapper(const function_wrapper&) = delete;
		function_wrapper(function_wrapper&) = delete;
		function_wrapper& operator=(const function_wrapper&) = delete;
	};

	class thread_pool {
	private:
		LISTING_6_7::threadsafe_queue<function_wrapper> work_queue;
		std::vector<std::thread> threads;
		std::atomic_bool done;

		void worker_thread() {
			while(!done) {
				function_wrapper task;
				if(work_queue.try_pop(task)) {
					task();
				} else {
					std::this_thread::yield();
				}
			}
		}

	public:
		thread_pool() : done(false) {
			unsigned const thread_count = std::thread::hardware_concurrency();
			try {
				for(unsigned i = 0; i < thread_count; ++i) {
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

		template<typename F>
		std::future<typename std::result_of<F()>::type> submit(F f) {
			typedef typename std::result_of<F()>::type result_type;
			std::packaged_task<result_type()> task(std::move(f));
			std::future<result_type> res(task.get_future());
			work_queue.push(std::move(task));
			return res;
		}
	};

	template<typename Iterator, typename T>
	struct accumulate_block {
		T operator()(Iterator first, Iterator last) {
			return std::accumulate(first, last, T());
		}
	};

	template<typename Iterator, typename T>
	T parallel_accumulate(Iterator first, Iterator last, T init) {
		unsigned long const length = std::distance(first, last);
		if (!length)
			return init;
		unsigned long const block_size = 25;
		unsigned long const num_blocks = (length + block_size -1) / block_size;
		std::vector<std::future<T> > futures(num_blocks - 1);
		thread_pool pool;

		Iterator block_start = first;
		for(unsigned long i = 0; i < (num_blocks - 1); ++i) {
			//T val;
			Iterator block_end = block_start;
			std::advance(block_end, block_size);
			futures[i] = pool.submit([=]{
				return accumulate_block<Iterator,T>()(block_start, block_end);
			});
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
		std::vector<int> v(100);
		std::iota(std::begin(v), std::end(v), 1);
		auto print = [](const int& n) { std::cout << " " << n; };
		std::for_each(v.begin(), v.end(), print);
		std::cout << '\n';

		int init = 0;
		int result = parallel_accumulate<std::vector<int>::iterator, int>(std::begin(v), std::end(v), init);
		std::cout << "parallel accumulate: " << result << std::endl << std::flush;
	}
}



#endif /* LISTING_9_2_H_ */
