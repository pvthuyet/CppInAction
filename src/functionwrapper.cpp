/*
 * functionwrapper.cpp
 *
 *  Created on: Aug 12, 2019
 *      Author: thuye
 */

#include "functionwrapper.h"

namespace tet {

	function_wrapper::function_wrapper() = default;
	function_wrapper::~function_wrapper() noexcept
	{
		// TODO Auto-generated destructor stub
	}

	function_wrapper::function_wrapper(function_wrapper&& other) :
			impl(std::move(other.impl))
	{
	}

	function_wrapper& function_wrapper::operator=(function_wrapper&& other)
	{
		impl = std::move(other.impl);
		return *this;
	}

	void function_wrapper::operator()()
	{
		if (impl) {
			impl->call();
		}
	}
} /* namespace tet */
