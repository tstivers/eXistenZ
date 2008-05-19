#pragma once

#define JSFUNCTIONCALL_MAX_ARITY 10
#include "jsvector.h"

namespace jsscript
{
	namespace jsfunctioncall_detail
	{
		#pragma region root_predicates

		#include <boost/type_traits/detail/bool_trait_def.hpp>
		BOOST_TT_AUX_BOOL_TRAIT_DEF1(arg_needs_root,T,true)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,unsigned char,false)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,bool,false)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,void,false)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,int,false)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,unsigned int,false)
		BOOST_TT_AUX_BOOL_TRAIT_CV_SPEC1(arg_needs_root,char,false)
		BOOST_TT_AUX_BOOL_TRAIT_DEF1(rval_needs_root,T,false)
		#include <boost/type_traits/detail/bool_trait_undef.hpp>

		#pragma endregion

		#pragma region struct root_impl
		
		// auto root structures for args and return value

		template<typename root_args, typename root_ret>
		struct root_impl
		{
			root_impl()
			{
				BOOST_STATIC_ASSERT(!"error: rooting implementation not supported");
			}
		};

		template<>
		struct root_impl<boost::mpl::bool_<true>, boost::mpl::bool_<false>>
		{
			JSContext* cx;
			inline root_impl(JSContext* cx, jsval& rval)
				: cx(cx)
			{
				JS_EnterLocalRootScope(cx);
			}

			inline ~root_impl()
			{
				JS_LeaveLocalRootScope(cx);
			}
		};

		template<>
		struct root_impl<boost::mpl::bool_<true>, boost::mpl::bool_<true>>
		{
			JSContext* cx;
			jsval& rval;
			inline root_impl(JSContext* cx, jsval& rval)
				: cx(cx), rval(rval)
			{
				JS_EnterLocalRootScope(cx);
			}

			inline ~root_impl()
			{
				JS_LeaveLocalRootScopeWithResult(cx, rval);
			}
		};

		template<>
		struct root_impl<boost::mpl::bool_<false>, boost::mpl::bool_<true>>
		{
			JSContext* cx;
			jsval& rval;
			inline root_impl(JSContext* cx, jsval& rval)
				: cx(cx), rval(rval)
			{
				JS_EnterLocalRootScope(cx);
			}

			inline ~root_impl()
			{
				JS_LeaveLocalRootScopeWithResult(cx, rval);
			}
		};

		template<>
		struct root_impl<boost::mpl::bool_<false>, boost::mpl::bool_<false>>
		{
			inline root_impl(JSContext* cx, jsval& rval)
			{
			}
		};

		#pragma endregion 

		#pragma region class jsfunctioncall_base

		template <int arity, typename T>
		class jsfunctioncall_base
		{
			jsfunctioncall_base()
			{
				BOOST_STATIC_ASSERT(!"arity not supported");
			}
		};

		template<typename T>
		class jsfunctioncall_base<0, T>
		{
			typedef typename root_impl<boost::mpl::false_, boost::mpl::bool_<jsfunctioncall_detail::rval_needs_root<typename boost::function_traits<T>::result_type>::value>> root_t;
		public:
			inline typename boost::function_traits<T>::result_type operator()(JSContext* cx, JSObject* par, jsval fun)
			{
				jsval argv, rval;
				root_t root(cx, rval);
				JS_CallFunctionValue(cx, par, fun, 0, &argv, &rval);
				return jsval_to_<boost::function_traits<T>::result_type>()(cx, rval); 
			}
		};

#define JSFUNCTIONCALL_BASE_TYPEDEF_ARG(z, n, text) \
			BOOST_PP_COMMA_IF(n) typename boost::call_traits<typename boost::function_traits<T>::arg ## n ##_type>::param_type arg ## n

#define JSFUNCTIONCALL_BASE_ASSIGN_ARG(z, n, text) \
			argv[BOOST_PP_DEC(n)] = to_jsval(cx, arg ## n);

#define JSFUNCTIONCALL_BASE_IMPL(z, n, text) \
		template <typename T> \
		class jsfunctioncall_base<n, T> \
		{ \
			typedef typename boost::mpl::count_if<boost::function_types::parameter_types<T>, jsfunctioncall_detail::arg_needs_root<boost::mpl::_>>::type arg_root_count; \
			typedef typename boost::mpl::if_<arg_root_count, boost::mpl::bool_<true>, boost::mpl::bool_<false>>::type args_need_root; \
			typedef typename root_impl<args_need_root, boost::mpl::bool_<jsfunctioncall_detail::rval_needs_root<typename boost::function_traits<T>::result_type>::value>> root_t; \
		public: \
			inline typename boost::function_traits<T>::result_type operator()(JSContext* cx, JSObject* par, jsval fun \
				BOOST_PP_REPEAT_FROM_TO_ ## z(1, BOOST_PP_INC(n), JSFUNCTIONCALL_BASE_TYPEDEF_ARG, null) \
				) \
			{ \
				jsval argv[n], rval; \
				root_t root(cx, rval); \
				BOOST_PP_REPEAT_FROM_TO_ ## z(1, BOOST_PP_INC(n), JSFUNCTIONCALL_BASE_ASSIGN_ARG, null) \
				JS_CallFunctionValue(cx, par, fun, n, &argv[0], &rval); \
				return jsval_to_<boost::function_traits<T>::result_type>()(cx, rval); \
			} \
		}; \

		BOOST_PP_REPEAT_FROM_TO(1, JSFUNCTIONCALL_MAX_ARITY, JSFUNCTIONCALL_BASE_IMPL, null)

		#pragma endregion
	}

	#pragma region to_jsval

	template <typename T>
	inline jsval to_jsval(JSContext* cx, T arg)
	{
		BOOST_STATIC_ASSERT(!"unable to convert arg to jsval");
	}

	inline jsval to_jsval(JSContext* cx, int arg)
	{
		return INT_TO_JSVAL(arg);
	}

	inline jsval to_jsval(JSContext* cx, const string& s)
	{
		return STRING_TO_JSVAL(JS_NewStringCopyZ(cx, s.c_str()));
	}

	inline jsval to_jsval(JSContext* cx, const char* c)
	{
		return STRING_TO_JSVAL(JS_NewStringCopyZ(cx, c));
	}

	inline jsval to_jsval(JSContext* cx, bool b)
	{
		return b ? JS_TRUE : JS_FALSE;
	}

	inline jsval to_jsval(JSContext* cx, float f)
	{
		jsval v;
		JS_NewNumberValue(cx, f, &v);
		return v;
	}

	inline jsval to_jsval(JSContext* cx, double d)
	{
		jsval v;
		JS_NewNumberValue(cx, d, &v);
		return v;
	}

	template <typename T>
	inline jsval to_jsval(JSContext* cx, const vector<T>& vec)
	{
		jsval a;
		JSObject* arr = JS_NewArrayObject(cx, 0, NULL);
		for(int i = 0; i < vec.size(); i++)
		{
			jsval v = to_jsval(cx, vec[i]);
			JS_SetElement(cx, arr, i, &v);
		}

		return OBJECT_TO_JSVAL(arr);
	}

	inline jsval to_jsval(JSContext* cx, const D3DXVECTOR3& vec)
	{
		return OBJECT_TO_JSVAL(jsvector::NewVector(cx, NULL, vec));
	}

	#pragma endregion

	#pragma region jsval_to_

	template <typename T>
	struct jsval_to_
	{
		inline T operator()(JSContext* cx, jsval v)
		{
			BOOST_STATIC_ASSERT(!"unable to convert arg from jsval");
		}
	};

	template<>
	struct jsval_to_<void>
	{
		inline void operator()(JSContext* cx, jsval v)
		{
		}
	};

	template<>
	struct jsval_to_<bool>
	{
		inline bool operator()(JSContext* cx, jsval v)
		{
			if(JSVAL_IS_BOOLEAN(v))
			{
				return JSVAL_TO_BOOLEAN(v) == JS_TRUE;
			}
			else
			{
				JSBool boolean;
				if(JS_ValueToBoolean(cx, v, &boolean))
					return boolean == JS_TRUE;
			}
			return false;
		}
	};

	template<>
	struct jsval_to_<string>
	{
		inline string operator()(JSContext* cx, jsval v)
		{
			if(JSVAL_IS_STRING(v))
			{
				return string(JS_GetStringBytes(JSVAL_TO_STRING(v)));
			}
			else if(JSString * s = JS_ValueToString(cx, v))
			{
				return string(JS_GetStringBytes(s));
			}
			else
				return "null";
		}
	};

	template<>
	struct jsval_to_<float>
	{
		inline float operator()(JSContext* cx, jsval v)
		{
			if(JSVAL_IS_INT(v))
			{
				return JSVAL_TO_INT(v);
			}
			else
			{
				jsdouble d;
				if(JS_ValueToNumber(cx, v, &d))
					return d;
				else
					return 0.0f;
			}
		}
	};

	template<>
	struct jsval_to_<double>
	{
		inline double operator()(JSContext* cx, jsval v)
		{
			if(JSVAL_IS_INT(v))
			{
				return JSVAL_TO_INT(v);
			}
			else
			{
				jsdouble d;
				if(JS_ValueToNumber(cx, v, &d))
					return d;
				else
					return 0.0;
			}
		}
	};

	template<>
	struct jsval_to_<int>
	{
		inline int operator()(JSContext* cx, jsval v)
		{
			if(JSVAL_IS_INT(v))
			{
				return JSVAL_TO_INT(v);
			}
			else
			{
				int32 i;
				if(JS_ValueToInt32(cx, v, &i))
					return i;
				else
					return 0;
			}
		}
	};

	#pragma endregion

	template <typename T>
	class jsfunctioncall :
		public jsfunctioncall_detail::jsfunctioncall_base<boost::function_traits<T>::arity, T>
	{
	};
}