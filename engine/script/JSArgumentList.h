#pragma once

namespace script {
	class JSArgumentList {
	public:
		JSArgumentList(JSContext* cx);
		~JSArgumentList();
		
		JSArgumentList& addParam(jsval val, bool rooted = false);
		JSArgumentList& addParam(int i);
		JSArgumentList& addParam(float f);
		JSArgumentList& addParam(double d);
		JSArgumentList& addParam(bool b);
		JSArgumentList& addParam(const char* str);
		JSArgumentList& addParam(string& str);
		
		void clear();
		int getCount();
		jsval* getArgv();
		
	private:		
		void addRoot(jsval& gcthing);	
	
	private:
		JSContext* cx;
		typedef vector<jsval> vec_jsval;
		vec_jsval roots;
		vec_jsval argv;		
	};


}