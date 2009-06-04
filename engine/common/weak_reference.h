#pragma once

template<typename T>
class weak_reference
{
	template<typename T>
	friend class weak_ref_provider;

public:	
	weak_reference()
	{}

	weak_reference(T* object)
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

	inline T* operator->() const { return (m_valid && *m_valid) ? m_object : NULL; }
	inline operator T* () const { return operator->(); }
	inline T* get() const { return operator->(); }

	inline bool operator=(T* object)
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
		
		return operator->();
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
	shared_ptr<bool> m_weak_ref_flag;
};