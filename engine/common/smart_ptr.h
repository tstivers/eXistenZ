namespace std {

	template <class T> 
	class smart_ptr {
	public:
		typedef T element_t;

	public:
		inline smart_ptr();
		inline smart_ptr(T* pointed);
		inline smart_ptr(const smart_ptr& other);
		inline ~smart_ptr();

		const T* c_ptr() { return m_pointed; };
		operator bool() { return m_pointed != 0; };
		inline smart_ptr& operator= (const smart_ptr& other);
		inline smart_ptr& operator= (T* other);
		T& operator* (void) const { return *m_pointed; };
		T* operator-> (void) const { return m_pointed; };
		bool operator== (const smart_ptr& other) const { return m_pointed == other.m_pointed; };
		bool operator== (const T* other) const { return m_pointed == other; };
		bool operator!= (const smart_ptr& other) const { return m_pointed != other.m_pointed; };
		bool operator!= (const T* other) const { return m_pointed != other; };
		bool operator< (const smart_ptr& other) const { return m_pointed < other.m_pointed; };
		bool operator< (const T* other) const { return m_pointed < other; };

	private:
		T* m_pointed; 
		unsigned int* m_counter; 
	};
};

template <class T> 
std::smart_ptr<T>::smart_ptr()
: m_pointed(NULL), m_counter(0)
{
}

template <class T> 
std::smart_ptr<T>::smart_ptr(T* pointed)
: m_pointed(pointed), m_counter(new unsigned int(1))
{
}

template <class T> 
std::smart_ptr<T>::smart_ptr(const smart_ptr<T>& other)
: m_pointed(other.m_pointed), m_counter(other.m_counter)
{
	if (m_counter) ++(*m_counter);
}

template <class T> 
std::smart_ptr<T>::~smart_ptr(void)
{
	if (m_counter) {
		--(*m_counter);
		if ( *m_counter == 0 ) {
			delete m_pointed;
			delete m_counter;
		}
	}
}

template <class T> 
std::smart_ptr<T>& std::smart_ptr<T>::operator=(const smart_ptr<T>& other)
{
	if (m_pointed) {
		if (m_counter) {
			--(*m_counter);
			if ( *m_counter == 0 ) {
				delete m_pointed;
				delete m_counter;
			}
		}
	}
	m_pointed = other.m_pointed;
	m_counter = other.m_counter;
	if (m_counter) ++(*m_counter);
	return *this;
}

template <class T> 
std::smart_ptr<T>& std::smart_ptr<T>::operator=(T* other)
{
	if (m_pointed) {
		if (m_counter) {
			--(*m_counter);
			if ( *m_counter == 0 ) {
				delete m_pointed;
				delete m_counter;
			}
		}
	}
	m_pointed = other;
	m_counter = new unsigned int(1);	
	return *this;
}