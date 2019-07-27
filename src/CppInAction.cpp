//============================================================================
// Name        : CppInAction.cpp
// Author      : thuyet pham
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "listing_6.1.h"
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	LISTING_6_1::threadsafe_stack<int> stk;
	//stk.push(1);
	//cout << *stk.pop() << endl;
	try {
		stk.pop();
	} catch (const exception& e) {
		cout << e.what();
	}
	return 0;
}
