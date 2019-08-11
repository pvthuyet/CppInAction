/*
 * Utility.h
 *
 *  Created on: Aug 11, 2019
 *      Author: thuye
 */

#ifndef UTILITY_H_
#define UTILITY_H_
#include <list>
#include <limits>
#include <random>

namespace tet {

	class Utility {
	public:
		Utility();
		virtual ~Utility();
		static std::list<int> random(int num, int min = 0, int max = std::numeric_limits<int>::max());
		template<class T>
		static T rand(T min, T max) {
		    std::random_device rd;
		    std::uniform_int_distribution<T> dist(min, max);
		    return dist(rd);
		}
	};

} /* namespace tet */

#endif /* UTILITY_H_ */
