/*
 * listing_7.2.h
 *
 *  Created on: Aug 6, 2019
 *      Author: thuye
 */

#ifndef LISTING_7_2_H_
#define LISTING_7_2_H_
#include <atomic>
namespace LISTING_7_2 {
	template<typename T>
	class lock_free_stack {
	private:
		struct node {
			T data;
			node* next;
			node(T const& data_):data(data_){}
		};
		std::atomic<node*> head;
		void push(T const& data) {
			node* const new_val = new node(data);
			new_val->next = head.load();
			while(!head.compare_exchange_weak(new_val->next, new_val));
		}

		void pop(T& result) {
			node* old_head = head.load();
			while(!head.compare_exchange_weak(old_head, old_head->next));
			result = old_head->data;
		}
	};
}



#endif /* LISTING_7_2_H_ */
