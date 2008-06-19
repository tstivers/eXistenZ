#pragma once

#include "jsfunctioncall.h"

namespace jsscript
{
	namespace jsfunction_detail
	{
#pragma region jsfunction_base

		class jsfunction_base
		{
		public:
			jsval fun;
			JSObject* par;
			JSContext* cx;
		};

		template <int arity, typename T>
		class jsfunction_baseN
		{
			jsfunction_baseN()
			{
				BOOST_STATIC_ASSERT(!"arity not supported");
			}
		};

		template<typename T>
		class jsfunction_baseN<0, T> : public jsfunction_base
		{
		public:
			inline typename jsfunctioncall<T>::result_type operator()()
			{
				return jsfunctioncall<T>()(cx, par, fun);
			}
		};

#define JSFUNCTION_BASEN_TYPEDEF_ARG(z, n, text) \
	BOOST_PP_COMMA_IF(BOOST_PP_DEC(n)) typename boost::call_traits<typename boost::function_traits<T>::arg ## n ##_type>::param_type arg ## n

#define JSFUNCTION_BASEN_FORWARD_ARG(z, n, text) \
	BOOST_PP_COMMA_IF(BOOST_PP_DEC(n)) arg ## n

#define JSFUNCTION_BASEN_IMPL(z, n, text) \
	template<typename T> \
	class jsfunction_baseN<n, T> : public jsfunction_base \
	{ \
	public: \
		inline typename boost::function_traits<T>::result_type operator()( \
				BOOST_PP_REPEAT_FROM_TO_ ## z(1, BOOST_PP_INC(n), JSFUNCTION_BASEN_TYPEDEF_ARG, null) \
																		 ) \
		{ \
			return jsfunctioncall<T>()(cx, par, fun, \
									   BOOST_PP_REPEAT_FROM_TO_ ## z(1, BOOST_PP_INC(n), JSFUNCTION_BASEN_FORWARD_ARG, null) \
									  ); \
		} \
	}; \
	 
		BOOST_PP_REPEAT_FROM_TO(1, JSFUNCTIONCALL_MAX_ARITY, JSFUNCTION_BASEN_IMPL, null)

#pragma endregion
	}

	template<typename T>
	class jsfunction :
				public jsfunction_detail::jsfunction_baseN<boost::function_traits<T>::arity, T>
	{
	public:
		jsfunction(JSContext* cx, JSObject* par, jsval fun)
		{
			this->cx = cx;
			this->fun = fun;
			this->par = par ? par : JS_GetParent(cx, JSVAL_TO_OBJECT(fun));
			acquire_locks();
		}

		jsfunction(JSContext* cx, jsval fun)
		{
			this->cx = cx;
			this->fun = fun;
			this->par = JS_GetParent(cx, JSVAL_TO_OBJECT(fun));
			acquire_locks();
		}

		jsfunction(JSContext* cx, const string& function_name)
		{
			this->fun = 0;
			this->cx = cx;
			JSObject* fo = script::GetObject(function_name);
			ASSERT(fo);
			if (JS_ObjectIsFunction(cx, fo))
			{
				this->fun = OBJECT_TO_JSVAL(fo);
				this->par = JS_GetParent(cx, JSVAL_TO_OBJECT(fun));
			}

			ASSERT(fun);
			acquire_locks();
		}

		void acquire_locks()
		{
			JS_AddRoot(cx, &fun);
			if (par)
				JS_AddRoot(cx, &par);
		}

		~jsfunction()
		{
			JS_RemoveRoot(cx, &fun);
			if (par)
				JS_RemoveRoot(cx, &par);
		}
	};
}