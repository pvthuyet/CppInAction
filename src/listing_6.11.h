/*
 * listing_6.11.h
 *	Threadsafe lookup table
 *  Created on: Aug 1, 2019
 *      Author: thuye
 */

#ifndef LISTING_6_11_H_
#define LISTING_6_11_H_

#include <functional>
#include <list>
#include <algorithm>
#include <string>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace LISTING_6_11 {
	template<typename Key, typename Value, typename Hash=std::hash<Key> >
	class threadsafe_lookup_table {
	private:
		class bucket_type {
		private:
			using bucket_value = std::pair<Key, Value>;
			using bucket_data = std::list<bucket_value>;
			using bucket_iterator = typename std::list<bucket_value>::iterator;

			// properties
			bucket_data data;
			mutable boost::shared_mutex mutex;

			bucket_iterator find_entry_for(Key const& key) {
				return std::find_if(std::begin(data), std::end(data),
						[&](bucket_value const& item) { return item.first == key; });
			}
		public:
			Value value_for(Key const& key, Value const& default_val) {
				boost::shared_lock_guard<boost::shared_mutex> lock(mutex);
				bucket_iterator const found_entry = find_entry_for(key);
				return ((found_entry == data.end()) ? default_val : found_entry->second);
			}

			void add_or_update_mapping(Key const& key, Value const& value) {
				boost::shared_lock_guard<boost::shared_mutex> lock(mutex);
				bucket_iterator const found_entry = find_entry_for(key);
				if (found_entry == data.end()) {
					data.emplace_back(std::make_pair(key, value));
				} else {
					found_entry->second = value;
				}
			}
			void remove_mapping(Key const& key) {
				boost::shared_lock_guard<boost::shared_mutex> lock(mutex);
				bucket_iterator const found_entry = find_entry_for(key);
				if (found_entry != data.end()) {
					data.erase(found_entry);
				}
			}
		};

		std::vector<std::unique_ptr<bucket_type> > buckets;
		Hash hasher;

		bucket_type& get_bucket(Key const& key) const {
			std::size_t const bucket_index = hasher(key) % buckets.size();
			return *buckets[bucket_index];
		}

	public:
		threadsafe_lookup_table(unsigned num_buckets = 19, Hash const& hash_=Hash()) :
			buckets(num_buckets), hasher(hash_) {
			for (unsigned i=0; i < num_buckets; i++) {
				buckets[i].reset(new bucket_type);
			}
		}
		threadsafe_lookup_table(const threadsafe_lookup_table&) = delete;
		threadsafe_lookup_table& operator=(const threadsafe_lookup_table&) = delete;

		Value value_for(Key const& key, Value const& default_value = Value()) const {
			return get_bucket(key).value_for(key, default_value);
		}

		void add_or_update_mapping(Key const& key, Value const& value) {
			get_bucket(key).add_or_update_mapping(key, value);
		}
		void remove_mapping(Key const& key) {
			get_bucket(key).remove_mapping(key);
		}
	};
	void test() {
		std::mutex gMutLog;
		threadsafe_lookup_table<std::string, double, std::hash<std::string> > tbtest;
		auto af = [&tbtest, &gMutLog](int sl) {
			constexpr int N = 10;
			for(int i = 0; i < N; i++) {
				std::srand(std::time(nullptr)); // use current time as seed for random generator
				int random_variable = std::rand();
				std::string key = std::to_string(random_variable % N);
				tbtest.add_or_update_mapping(key, random_variable);
				{
					std::lock_guard<std::mutex> lock(gMutLog);
					std::cout << "Thread " << std::this_thread::get_id() << " add\t" << "("<< key <<"," << random_variable <<")" << std::endl << std::flush;
				}
				std::this_thread::sleep_for(std::chrono::seconds(sl));
			}
		};
		std::thread t1(af, 1);
		std::thread t2(af, 2);
		std::thread t3(af, 3);
		t1.join();
		t2.join();
		t3.join();
	}
}



#endif /* LISTING_6_11_H_ */
