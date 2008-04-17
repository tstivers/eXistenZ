#pragma once

namespace jsscript
{
	inline void JSConvert(const int& in, jsval& out)
	{
		out = INT_TO_JSVAL(in);
	}

	template <typename _T1>
	class JSArgs
	{
	public:
		jsval argv;
		static const int arity = 1;
		JSArgs(_T1 argv1)
		{
			JSConvert(argv1, argv);
		}
		~JSArgs() {};
	};

	template <typename _T1, typename _T2>
	class JSArgs
	{
	public:
		jsval argv[2];
		static const int arity = 2;
		JSArgs(_T1 argv1, _T2 argv2)
		{
			JSConvert(argv1, argv[0]);
			JSConvert(argv2, argv[1]);
		}
		~JSArgs() {};
	};

	bool CallJSFunction(JSContext* cx, JSObject* thisp, jsval fun)
	{
		jsval rval;
		if(JS_CallFunctionValue(cx, thisp, fun, 0, NULL, &rval))
			return true;

		return false;
	}

	template <typename _RET>
	bool CallJSFunction(JSContext* cx, JSObject* thisp, jsval fun, _RET &result)
	{
		jsval rval;
		if(JS_CallFunctionValue(cx, thisp, fun, 0, NULL, &rval))
		{
			JSConvert(rval, result);
			return true;
		}

		return false;
	}

	template <typename _RET, typename _ARG1>
	bool CallJSFunction(JSContext* cx, JSObject* thisp, jsval fun, const _ARG1 &arg1, _RET &result)
	{
		jsval rval;
		JSArgs<_ARG1> args(arg1);

		if(JS_CallFunctionValue(cx, thisp, fun, 1, &args.argv, &rval))
		{
			JSConvert(rval, result);
			return true;
		}

		return false;
	}

	template <typename _RET, typename _ARG1, typename _ARG2>
	bool CallJSFunction(JSContext* cx, JSObject* thisp, jsval fun, const _ARG1 &arg1, const _ARG2 &arg2, _RET &result)
	{
		jsval rval;
		JSArgs<_ARG1, _ARG2> args(arg1, arg2);

		if(JS_CallFunctionValue(cx, thisp, fun, 1, &args.argv, &rval))
		{
			JSConvert(rval, result);
			return true;
		}

		return false;
	}
}