#pragma once

template<typename T>
class weak_reference
{
public:	
	weak_reference()
	{}

	explicit weak_reference(T* object)
		: m_object(object)
	{
		if(object)
		{
			ASSERT(dynamic_cast<T::weak_ref_type*>(object));
			if(!((T::weak_ref_type*)object)->m_weak_ref_flag)
				((T::weak_ref_type*)object)->m_weak_ref_flag = make_shared<bool>(true);
			m_valid = ((T::weak_ref_type*)object)->m_weak_ref_flag;
		}
	}

	inline T* get() const { return (m_valid && *m_valid) ? m_object : NULL; }
	inline T* operator->() const { ASSERT(operator bool()); return get(); }
	inline operator bool() const { return m_valid && *m_valid && m_object; }

	weak_reference<T>& operator=(T* object)
	{
		m_object = object;
		if(object)
		{
			ASSERT(dynamic_cast<T::weak_ref_type*>(object));
			if(!((T::weak_ref_type*)object)->m_weak_ref_flag)
				((T::weak_ref_type*)object)->m_weak_ref_flag = make_shared<bool>(true);
			m_valid = ((T::weak_ref_type*)object)->m_weak_ref_flag;
		}
		else
			m_valid.reset();
		
		return *this;
	}

private:
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

	~weak_ref_provider()
	{
		if(m_weak_ref_flag)
			*m_weak_ref_flag = false;
	}

private:
	mutable shared_ptr<bool> m_weak_ref_flag;
};