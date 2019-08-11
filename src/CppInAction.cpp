
//============================================================================
// Name        : CppInAction.cpp
// Author      : thuyet pham
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifndef _DEBUG
#define _DEBUG
#endif
#include <iostream>
#include "logger.h"
#include "listing_5.4.h"
#include "listing_5.5.h"
#include "listing_6.1.h"
#include "listing_6.2.h"
#include "listing_6.3.h"
#include "listing_6.4.h"
#include "listing_6.5.h"
#include "listing_6.6.h"
#include "listing_6.7.h"
//#include "listing_6.11.h"
#include "listing_6.13.h"
#include "listing_7.1.h"
#include "listing_7.2.h"
#include "listing_9.1.h"
#include "listing_9.2.h"
#include "listing_9.3.h"
#include "listing_9.5.h"

#include <cassert>
#include <vector>
#include "Utility.h"

using namespace std;
tet::logger gLogger;

int main() {
	int r = tet::Utility::rand(0, 10);
	std::cout << r << std:: endl;
	//LISTING_5_4::test();
	//LISTING_5_5::test();
	//LISTING_6_1::test();
	//LISTING_6_2::test();
	//LISTING_6_3::test();
	//LISTING_6_4::test();
	//LISTING_6_5::test();
	//LISTING_6_6::test();
	//LISTING_6_7::test();
	//LISTING_6_11::test();
	//LISTING_6_13::test();
	//LISTING_7_1::test();
	//LISTING_9_1::test();
	//LISTING_9_3::test();
	//LISTING_9_5::test();
	std::cout << "End!";

	return 0;
}

/*
#include <random>
#include <iostream>
#include <memory>
#include <functional>
#include <future>


void f(int n1, int n2, int n3, const int& n4, int n5)
{
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}

int g(int n1)
{
    return n1;
}
class abc {
public:
template<class Fn, class... Args>
		auto submit(Fn&& f, Args&&... args)
			-> std::future<std::result_of_t<Fn(Args...)> >
		{
			using return_type = std::result_of_t<Fn(Args...)>;
			auto task = std::make_shared<std::packaged_task<return_type()> >
			(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
			);
			std::future<return_type> res = task->get_future();
			(*task)();


			return res;
		}
};

struct Foo {
    int print_sum(int n1, int n2)
    {
        std::cout << "n1 = " << n1 << ", n2 = " << n2 << std::endl;
        std::cout << n1+n2 << '\n';
        return n1 + n2;
    }

    int data = 10;
    std::list<int> test(std::list<int> a) {
    	return std::list<int> {1,3,4};
    }

    void aaa() {
    	abc a;
    	std::list<int> nlist {1,3,4};
    	auto f = std::bind(&Foo::test, this, std::placeholders::_1);
    	a.submit(std::move(f), std::move(nlist));
    }
};




int main()
{
    //using namespace std::placeholders;  // for _1, _2, _3...


    // bind to a pointer to member function
    //Foo foo;
    //auto f3 = std::bind(&Foo::print_sum, &foo, std::placeholders::_1, std::placeholders::_2);

    //auto f4 = std::bind(f3, 1, 2);
    //f4();
   //auto fut = submit(f3, 1, 3);
   //std::cout << std::endl << fut.get() << std::endl;
	Foo foo;
	foo.aaa();

    return 0;
}

*/
