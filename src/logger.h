/*
 * log.h
 *
 *  Created on: Aug 10, 2019
 *      Author: thuye
 */

#ifndef LOGGER_H_
#define LOGGER_H_
#include <mutex>
namespace tet {
	class logger {
	private:
		std::mutex m_mut;
	public:
		logger();
		void debug(int a, int b);
		virtual ~logger();
	};
}
#endif /* LOGGER_H_ */
