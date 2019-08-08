/*
 * listing_8.3.h
 *
 *  Created on: Aug 8, 2019
 *      Author: thuye
 */

#ifndef LISTING_8_3_H_
#define LISTING_8_3_H_
#include <thread>
#include <vector>
namespace LISTING_8_3 {
	class join_threads {
	private:
		std::vector<std::thread> &threads;
	public:
		explicit join_threads(std::vector<std::thread> &threads_) :
			threads(threads_) {}
		virtual ~join_threads() {
			for(unsigned i = 0; i < threads.size(); ++i) {
				if (threads[i].joinable()) {
					threads[i].join();
				}
			}
		}
	};
}



#endif /* LISTING_8_3_H_ */
