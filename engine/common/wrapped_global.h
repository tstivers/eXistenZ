#pragma once

template<typename T>
class wrapped_global
{
public:
	T* operator->()
	{
		static T g;
		return &g;
	}
};