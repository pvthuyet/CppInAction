/*
 * listing_9.6.h
 *
 *  Created on: Aug 11, 2019
 *      Author: thuye
 */

#ifndef LISTING_9_6_H_
#define LISTING_9_6_H_
#include "listing_6.7.h"
#include "listing_8.3.h"
#include <atomic>
#include <functional>
#include <queue>
#include <memory>
#include <thread>
#include <vector>
#include <future>
#include "listing_9.2.h"

extern tet::logger gLogger;
namespace LISTING_9_6 {
	// define new data type
	using local_queue_type = std::queue<std::function<void()> >;

	class thread_pool {
	private:
		// data members
		std::atomic_bool done;
		LISTING_6_7::threadsafe_queue<std::function<void()> > pool_work_queue;
		static thread_local std::unique_ptr<local_queue_type> local_work_queue;

		// threads
		std::vector<std::thread> threads;
		LISTING_8_3::join_threads joiner;

		// private method.
		void worker_thread() {
			std::ostringstream os;
			os << "Thread " << std::this_thread::get_id() << " Initialize...: \n";
			gLogger.debug(os.str());

			local_work_queue.reset(new local_queue_type());

			while(!done) {
				run_pending_task();
			}
		}

	public:
		thread_pool(unsigned numThread = std::thread::hardware_concurrency()) : done(false), joiner(threads) {
			try {
				for(unsigned i = 0; i < numThread; ++i) {
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

		void run_pending_task() {
			std::ostringstream os;
			os << "Thread " << std::this_thread::get_id();
			std::function<void()> task;
			if(local_work_queue && !local_work_queue->empty()) {
				gLogger.debug(os.str() + " POP local_work_queue\n");
				task = std::move(local_work_queue->front());
				local_work_queue->pop();
				task();
			}
			else if(pool_work_queue.try_pop(task)) {
				gLogger.debug(os.str() + " POP pool_work_queue\n");
				task();
			} else {
				gLogger.debug(os.str() + " sleeping...\n");
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}

		template<class Fn, class... Args>
		auto submit(Fn&& f, Args&&... args)
			-> std::future<std::result_of_t<Fn(Args...)> >
		{
			using return_type = std::result_of_t<Fn(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
			(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
			);
			std::future<return_type> res = task->get_future();
			if(local_work_queue) {
				std::ostringstream os;
				os << "Thread " << std::this_thread::get_id() << " push local_work_queue...: \n";
				gLogger.debug(os.str());
				local_work_queue->push([task](){
					(*task)();
				});
			} else {
				std::ostringstream os;
				os << "Thread " << std::this_thread::get_id() << " push pool_work_queue...: \n";
				gLogger.debug(os.str());

				pool_work_queue.push([task](){
					(*task)();
				});
			}

			return res;
		}
	};

	// Must initialize for thread_local variable.
	thread_local std::unique_ptr<local_queue_type> thread_pool::local_work_queue = nullptr;

	template<typename T>
	struct sorter {
		LISTING_9_6::thread_pool pool;
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
		unsigned long const num_threads = 2;//std::thread::hardware_concurrency();
		unsigned long const num_blocks = (length + block_size -1) / (block_size );
		std::vector<std::future<T> > futures(num_blocks - 1);
		LISTING_9_6::thread_pool pool(num_threads);

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

			case 'a':
				{
					constexpr int N = 1'000;
					int sz;
					std::cout << std::endl;
					std::cout << "Enter number of elements (default " << N << " ): ";
					std::cin >> sz;
					if (sz < 2 ) {
						sz = N;
					}
					std::vector<int> v(sz);
					std::iota(std::begin(v), std::end(v), 1);
					std::cout << std::endl;
					int init = 0;
					int result = parallel_accumulate<std::vector<int>::iterator, int>(std::begin(v), std::end(v), init);
					std::cout << "parallel accumulate: "
							<< v[0] << " + "
							<< v[1] << " + ... + "
							<< v[sz-1] << " = "
							<< result << std::endl << std::flush;
				}

			default:
				break;
			}
		}
	}
}



#endif /* LISTING_9_6_H_ */
