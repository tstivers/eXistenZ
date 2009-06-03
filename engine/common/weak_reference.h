#pragma once

template<typename T>
class weak_reference
{
	template<typename T>
	friend class weak_ref_provider;

public:	
	weak_reference()
		: m_object(NULL)
	{}

	weak_reference(T* object)
		: m_object(object)
	{
		if(object)
		{
			ASSERT(dynamic_cast<T::weak_ref_type*>(object));
			m_valid = ((T::weak_ref_type*)object)->m_weak_ref_flag;
		}
	}

	inline T* operator->() const { return (m_valid && *m_valid) ? m_object : NULL; }
	inline operator T* () const { return operator->(); }
	inline T* get() const { return operator->(); }

	inline bool operator=(T* object)
	{
		m_object = object;
		if(object)
		{
			ASSERT(dynamic_cast<T::weak_ref_type*>(object));
			m_valid = ((T::weak_ref_type*)object)->m_weak_ref_flag;
		}
		else
			m_valid.reset();
		
		return operator->();
	}

private:
	weak_reference(T* object, shared_ptr<bool>& flag)
		: m_object(object), m_valid(flag)
	{}

	shared_ptr<bool> m_valid;
	T* m_object;
};

template<typename T>
class weak_ref_provider
{
	template<typename T>
	friend class weak_reference;

public:
	typedef weak_ref_provider<T> weak_ref_type;

	weak_ref_provider()
		: m_weak_ref_flag(new bool(true))
	{}

	~weak_ref_provider()
	{
		*m_weak_ref_flag = false;
	}

	template<typename U>
	weak_reference<U> getWeakReference()
	{
		return weak_reference<U>(this, m_weak_ref_flag)
	}

private:
	shared_ptr<bool> m_weak_ref_flag;
};