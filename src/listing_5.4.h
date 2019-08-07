/*
 * listing_5.4.h
 *
 *  Created on: Aug 4, 2019
 *      Author: Dragons' Den
 */

#ifndef LISTING_5_4_H_
#define LISTING_5_4_H_
#include <thread>
#include <atomic>
#include <assert.h>

namespace LISTING_5_4 {
	std::atomic<bool> x,y;
	std::atomic<int> z;

	void write_x() {
		x.store(true, std::memory_order_seq_cst);
	}
	void write_y() {
		y.store(true, std::memory_order_seq_cst);
	}
	void read_x_then_y() {
		while(!x.load(std::memory_order_seq_cst));
		if (y.load(std::memory_order_seq_cst))
			z++;
	}
	void read_y_then_x() {
		while(!y.load(std::memory_order_seq_cst));
		if (x.load(std::memory_order_seq_cst))
			z++;
	}
	void test() {
		x = false;
		y = false;
		z= 0;
		std::thread a(write_x);
		std::thread b(write_y);
		std::thread c(read_x_then_y);
		std::thread d(read_y_then_x);
		a.join();
		b.join();
		c.join();
		d.join();
		assert(z !=0);
	}
}



#endif /* LISTING_5_4_H_ */
