#pragma once

namespace script
{
	namespace detail
	{
		template<int arity, typename return_type, typename function_type, typename function_type function_pointer>
		struct JSNativeCallMemberBase
		{
			JSNativeCallMemberBase()
			{
				BOOST_STATIC_ASSERT(!"arity not supported");
			}
		};

		template<typename function_type, typename function_type function_pointer>
		struct JSNativeCallMemberBase<1, void, function_type, function_pointer>
		{
			typedef typename function_types::components<function_type, boost::remove_pointer<boost::add_pointer<mpl::placeholders::_>>> component_types;
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type;
			typedef typename mpl::at<component_types, mpl::int_<1>>::type wrapped_type;
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
			{
				wrapped_type* e = GetReserved<wrapped_type>(cx, obj);
				(e->*function_pointer)();
				*rval = JSVAL_VOID;
				return JS_TRUE;
			}
		};

		template<typename return_type, typename function_type, typename function_type function_pointer>
		struct JSNativeCallMemberBase<1, return_type, function_type, function_pointer>
		{
			typedef typename function_types::components<function_type, boost::remove_pointer<boost::add_pointer<mpl::placeholders::_>>> component_types;
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type;
			typedef typename mpl::at<component_types, mpl::int_<1>>::type wrapped_type;
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
			{
				wrapped_type* e = GetReserved<wrapped_type>(cx, obj);
				*rval = jsscript::to_jsval(cx, (e->*function_pointer)());
				return JS_TRUE;
			}
		};

	#define JSNATIVECALLMEMBER_ARG_TYPEDEF(z, n, text) \
		typedef typename mpl::at<component_types, mpl::int_<n + 2>>::type arg ## n ## _type; \
		typedef typename boost::remove_reference<arg ## n ## _type>::type arg ## n ## _type2; \
		typedef typename boost::remove_const<arg## n ## _type2>::type arg ## n ## _type3;

	#define JSNATIVECALLMEMBER_ARG_ASSIGN(z, n, text) \
		arg ## n ## _type3 arg ## n; \
		if(!jsscript::jsval_to_(cx, argv[ n ], &arg ## n)) \
		{ \
			JS_ReportError(cx, "ERROR: unable to decode arg[" #n "]"); \
			return JS_FALSE; \
		}

	#define JSNATIVECALLMEMBER_ARG_CALL(z, n, text) \
		BOOST_PP_COMMA_IF(n) arg ## n

	#define JSNATIVECALLMEMBER_BASE_VOID(z, n, text) \
		template<typename function_type, typename function_type function_pointer> \
		struct JSNativeCallMemberBase<n + 2, void, function_type, function_pointer> \
		{ \
			typedef typename function_types::components<function_type, boost::remove_pointer<boost::add_pointer<mpl::placeholders::_>>> component_types; \
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type; \
			typedef typename mpl::at<component_types, mpl::int_<1>>::type wrapped_type; \
			BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_TYPEDEF, null) \
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval) \
			{ \
				if (argc != (n + 1)) \
				{ \
				JS_ReportError(cx, "ERROR: incorrect arg count: expected %i, got %i", n + 1, argc); \
					return JS_FALSE; \
				} \
				wrapped_type* e = GetReserved<wrapped_type>(cx, obj); \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_ASSIGN, null) \
				(e->*function_pointer)( \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_CALL, null) \
				); \
				*rval = JSVAL_VOID; \
				return JS_TRUE; \
			} \
		}; \

		BOOST_PP_REPEAT_FROM_TO(0, 10, JSNATIVECALLMEMBER_BASE_VOID, NULL)

	#define JSNATIVECALLMEMBER_BASE(z, n, text) \
		template<typename return_type, typename function_type, typename function_type function_pointer> \
		struct JSNativeCallMemberBase<n + 2, return_type, function_type, function_pointer> \
		{ \
			typedef typename function_types::components<function_type, boost::remove_pointer<boost::add_pointer<mpl::placeholders::_>>> component_types; \
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type; \
			typedef typename mpl::at<component_types, mpl::int_<1>>::type wrapped_type; \
			BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_TYPEDEF, null) \
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval) \
			{ \
				if (argc != (n + 1)) \
				{ \
				JS_ReportError(cx, "ERROR: incorrect arg count: expected %i, got %i", n + 1, argc); \
					return JS_FALSE; \
				} \
				wrapped_type* e = GetReserved<wrapped_type>(cx, obj); \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_ASSIGN, null) \
				*rval = jsscript::to_jsval(cx, (e->*function_pointer)( \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALLMEMBER_ARG_CALL, null) \
				)); \
				return JS_TRUE; \
			} \
		}; \

		BOOST_PP_REPEAT_FROM_TO(0, 10, JSNATIVECALLMEMBER_BASE, NULL)

		template<int arity, typename return_type, typename function_type, typename function_type function_pointer>
		struct JSNativeCallBase
		{
			JSNativeCallBase()
			{
				BOOST_STATIC_ASSERT(!"arity not supported");
			}
		};

		template<typename function_type, typename function_type function_pointer>
		struct JSNativeCallBase<0, void, function_type, function_pointer>
		{
			typedef typename function_types::components<function_type> component_types;
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type;
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
			{
				(*function_pointer)();
				*rval = JSVAL_VOID;
				return JS_TRUE;
			}
		};

		template<typename return_type, typename function_type, typename function_type function_pointer>
		struct JSNativeCallBase<0, return_type, function_type, function_pointer>
		{
			typedef typename function_types::components<function_type> component_types;
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type;
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
			{
				*rval = jsscript::to_jsval(cx, (*function_pointer)());
				return JS_TRUE;
			}
		};

	#define JSNATIVCECALL_ARG_TYPEDEF(z, n, text) \
		typedef typename mpl::at<component_types, mpl::int_<n + 1>>::type arg ## n ## _type; \
		typedef typename boost::remove_reference<arg ## n ## _type>::type arg ## n ## _type2; \
		typedef typename boost::remove_const<arg## n ## _type2>::type arg ## n ## _type3;

	#define JSNATIVCECALL_ARG_ASSIGN(z, n, text) \
		arg ## n ## _type3 arg ## n; \
		if(!jsscript::jsval_to_(cx, argv[ n ], &arg ## n)) \
		{ \
			JS_ReportError(cx, "ERROR: unable to decode arg[" #n "]"); \
			return JS_FALSE; \
		}

	#define JSNATIVECALL_ARG_CALL(z, n, text) \
		BOOST_PP_COMMA_IF(n) arg ## n

	#define JSNATIVE_CALL_BASE_VOID(z, n, text) \
		template<typename function_type, typename function_type function_pointer> \
		struct JSNativeCallBase<n + 1, void, function_type, function_pointer> \
		{ \
			typedef typename function_types::components<function_type> component_types; \
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type; \
			BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVCECALL_ARG_TYPEDEF, null) \
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval) \
			{ \
				if (argc != (n + 1)) \
				{ \
				JS_ReportError(cx, "ERROR: incorrect arg count: expected %i, got %i", n + 1, argc); \
					return JS_FALSE; \
				} \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVCECALL_ARG_ASSIGN, null) \
				(*function_pointer)( \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALL_ARG_CALL, null) \
				); \
				*rval = JSVAL_VOID; \
				return JS_TRUE; \
			} \
		}; \

		BOOST_PP_REPEAT_FROM_TO(0, 10, JSNATIVE_CALL_BASE_VOID, NULL)

	#define JSNATIVE_CALL_BASE(z, n, text) \
		template<typename return_type, typename function_type, typename function_type function_pointer> \
		struct JSNativeCallBase<n + 1, return_type, function_type, function_pointer> \
		{ \
			typedef typename function_types::components<function_type> component_types; \
			typedef typename mpl::at<component_types, mpl::int_<0>>::type result_type; \
			BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVCECALL_ARG_TYPEDEF, null) \
			JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval) \
			{ \
				if (argc != (n + 1)) \
				{ \
				JS_ReportError(cx, "ERROR: incorrect arg count: expected %i, got %i", n + 1, argc); \
					return JS_FALSE; \
				} \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVCECALL_ARG_ASSIGN, null) \
				*rval = jsscript::to_jsval(cx, (*function_pointer)( \
				BOOST_PP_REPEAT_FROM_TO_ ## z(0, BOOST_PP_INC(n), JSNATIVECALL_ARG_CALL, null) \
				)); \
				return JS_TRUE; \
			} \
		}; \

		BOOST_PP_REPEAT_FROM_TO(0, 10, JSNATIVE_CALL_BASE, NULL)

		#undef JSNATIVECALLMEMBER_ARG_TYPEDEF
		#undef JSNATIVECALLMEMBER_ARG_ASSIGN
		#undef JSNATIVECALLMEMBER_ARG_CALL
		#undef JSNATIVECALLMEMBER_BASE_VOID
		#undef JSNATIVECALLMEMBER_BASE
		#undef JSNATIVCECALL_ARG_TYPEDEF
		#undef JSNATIVCECALL_ARG_ASSIGN
		#undef JSNATIVECALL_ARG_CALL
		#undef JSNATIVE_CALL_BASE_VOID
		#undef JSNATIVE_CALL_BASE

		template<typename function_type, typename function_type function_pointer>
		struct JSNativeCallSelector 
		{
			typedef typename mpl::if_c<function_types::is_member_function_pointer<function_type>::value,
				JSNativeCallMemberBase<
					function_types::function_arity<function_type>::value,
					typename function_types::result_type<function_type>::type,
					function_type, 
					function_pointer>,
				JSNativeCallBase<
					function_types::function_arity<function_type>::value,
					typename function_types::result_type<function_type>::type,
					function_type, 
					function_pointer>
			>::type call_base_type;

			inline JSBool operator()(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
			{
				return call_base_type()(cx, obj, argc, argv, rval);
			}
		};
	}
	
	#define WRAP_FASTNATIVE(x) (JSFastNativeCall<BOOST_TYPEOF(&x), &x>)
	#define WRAP_NATIVE(x) (JSSlowNativeCall<BOOST_TYPEOF(&x), &x>)

	template<typename function_type, typename function_type function_pointer>
	JSBool JSSlowNativeCall(JSContext* cx, JSObject* obj, int argc, jsval* argv, jsval* rval)
	{
		return detail::JSNativeCallSelector<function_type, function_pointer>()(
			cx, 
			obj,
			argc,
			argv, 
			rval);
	}


	template<typename function_type, typename function_type function_pointer>
	JSBool JSFastNativeCall(JSContext *cx, uintN argc, jsval *vp)
	{
		jsval ret = JSVAL_VOID;
		JSBool ok = detail::JSNativeCallSelector<function_type, function_pointer>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
			JS_SET_RVAL(cx, vp, ret);

		return ok;
	}

	template<typename function_type1, typename function_type1 function_pointer1,
			typename function_type2, typename function_type2 function_pointer2>
	JSBool JSFastNativeCall2(JSContext *cx, uintN argc, jsval *vp)
	{
		jsval ret = JSVAL_VOID;
		JSBool ok = detail::JSNativeCallSelector<function_type1, function_pointer1>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
		{
			JS_SET_RVAL(cx, vp, ret);
			return JS_TRUE;
		}

		JSBool ok = detail::JSNativeCallSelector<function_type2, function_pointer2>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
		{
			JS_SET_RVAL(cx, vp, ret);
			return JS_TRUE;
		}

		return JS_FALSE;
	}

	template<typename function_type1, typename function_type1 function_pointer1,
		typename function_type2, typename function_type2 function_pointer2,
		typename function_type3, typename function_type3 function_pointer3>

		JSBool JSFastNativeCall3(JSContext *cx, uintN argc, jsval *vp)
	{
		jsval ret = JSVAL_VOID;
		JSBool ok = detail::JSNativeCallSelector<function_type1, function_pointer1>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
		{
			JS_SET_RVAL(cx, vp, ret);
			return ok;
		}

		JSBool ok = detail::JSNativeCallSelector<function_type2, function_pointer2>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
		{
			JS_SET_RVAL(cx, vp, ret);
			return ok;
		}

		JSBool ok = detail::JSNativeCallSelector<function_type3, function_pointer3>()(
			cx, 
			JS_THIS_OBJECT(cx, vp),
			argc,
			JS_ARGV(cx, vp), 
			&ret);

		if(ok)
		{
			JS_SET_RVAL(cx, vp, ret);
			return ok;
		}

		return ok;
	}
} // namespace jsnative