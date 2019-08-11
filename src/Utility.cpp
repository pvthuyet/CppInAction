/*
 * Utility.cpp
 *
 *  Created on: Aug 11, 2019
 *      Author: thuye
 */

#include "Utility.h"

namespace tet {
	Utility::Utility() = default;
	Utility::~Utility() = default;
	std::list<int> Utility::random(int num, int min, int max) {
	    std::random_device rd;
	    std::list<int> lst;
	    std::uniform_int_distribution<int> dist(min, max);
	    for (int n = 0; n < num; ++n) {
	    	lst.emplace_front(dist(rd)); // note: demo only: the performance of many
	                          	  	  	 // implementations of random_device degrades sharply
	                          	  	  	 // once the entropy pool is exhausted. For practical use
	                          	  	  	 // random_device is generally only used to seed
	                          	  	  	 // a PRNG such as mt19937
	    }
	    return lst;
	}

} /* namespace tet */
