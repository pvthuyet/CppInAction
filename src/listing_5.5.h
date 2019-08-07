/*
 * listing_5.5.h
 *
 *  Created on: Aug 4, 2019
 *      Author: Dragons' Den
 */

#ifndef LISTING_5_5_H_
#define LISTING_5_5_H_
#include <thread>
#include <atomic>
#include <assert.h>

namespace LISTING_5_5 {
	std::atomic<bool> x,y;
	std::atomic<int> z;
	void write_x_then_y() {
		x.store(true,std::memory_order_relaxed);
		y.store(true,std::memory_order_relaxed);
	}
	void read_y_then_x() {
		while(!y.load(std::memory_order_relaxed));
		if (x.load(std::memory_order_relaxed))
			z++;
	}
	void test() {
		x=false;
		y=false;
		z=0;
		std::thread a(write_x_then_y);
		std::thread b(read_y_then_x);
		a.join();
		b.join();
		assert(z!=0);
	}
}



#endif /* LISTING_5_5_H_ */
