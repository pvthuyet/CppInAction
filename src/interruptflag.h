/*
 * interruptflag.h
 *
 *  Created on: Sep 19, 2019
 *      Author: thuye
 */

#ifndef INTERRUPTFLAG_H_
#define INTERRUPTFLAG_H_

namespace LISTING_9_9 {

class interrupt_flag {
public:
	interrupt_flag();
	virtual ~interrupt_flag();
	void set();
	bool is_set() const;
};

} /* namespace LISTING_9_9 */

#endif /* INTERRUPTFLAG_H_ */
