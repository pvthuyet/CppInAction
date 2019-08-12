/*
 * functionwrapper.h
 *
 *  Created on: Aug 12, 2019
 *      Author: thuye
 */

#ifndef FUNCTIONWRAPPER_H_
#define FUNCTIONWRAPPER_H_
#include <memory>
namespace tet {

class function_wrapper {
private:
	struct impl_base {
		virtual void call() = 0;
		virtual ~impl_base() {}
	};
	template<class F>
	struct impl_type: impl_base {
		F f;
		impl_type(F&& f_) : f(std::move(f_)) {}
		void call() { f(); }
	};

	// data members
	std::unique_ptr<impl_base> impl;

public:
	function_wrapper();
	template<class F>
	function_wrapper(F&& f) :
		impl(std::make_unique<impl_type<F> >(std::forward<F>(f)))
		{
		}

	function_wrapper(function_wrapper&& other);
	function_wrapper& operator=(function_wrapper&& other);

	virtual ~function_wrapper() noexcept;

	void operator()();

	// delete methods
	function_wrapper(const function_wrapper&) = delete;
	function_wrapper(function_wrapper&) = delete;
	function_wrapper& operator=(const function_wrapper&) = delete;
};

} /* namespace tet */

#endif /* FUNCTIONWRAPPER_H_ */
