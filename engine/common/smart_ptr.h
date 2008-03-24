namespace std {

	template <class T> 
	class shared_ptr {
	public:
		typedef T element_t;

	public:
		inline shared_ptr();
		inline shared_ptr(T* pointed);
		inline shared_ptr(const shared_ptr& other);
		inline ~shared_ptr();

		const T* c_ptr() { return m_pointed; };
		operator bool() { return m_pointed != 0; };
		inline shared_ptr& operator= (const shared_ptr& other);
		inline shared_ptr& operator= (T* other);
		T& operator* (void) const { return *m_pointed; };
		T* operator-> (void) const { return m_pointed; };
		bool operator== (const shared_ptr& other) const { return m_pointed == other.m_pointed; };
		bool operator== (const T* other) const { return m_pointed == other; };
		bool operator!= (const shared_ptr& other) const { return m_pointed != other.m_pointed; };
		bool operator!= (const T* other) const { return m_pointed != other; };
		bool operator< (const shared_ptr& other) const { return m_pointed < other.m_pointed; };
		bool operator< (const T* other) const { return m_pointed < other; };

	private:
		T* m_pointed; 
		unsigned int* m_counter; 
	};
};

template <class T> 
shared_ptr<T>::shared_ptr()
: m_pointed(NULL), m_counter(0)
{
}

template <class T> 
shared_ptr<T>::shared_ptr(T* pointed)
: m_pointed(pointed), m_counter(new unsigned int(1))
{
}

template <class T> 
shared_ptr<T>::shared_ptr(const shared_ptr<T>& other)
: m_pointed(other.m_pointed), m_counter(other.m_counter)
{
	if (m_counter) ++(*m_counter);
}

template <class T> 
shared_ptr<T>::~shared_ptr(void)
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
shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr<T>& other)
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
shared_ptr<T>& shared_ptr<T>::operator=(T* other)
{
	if (m_counter) {
		--(*m_counter);
		if ( *m_counter == 0 ) {
			delete m_pointed;
			delete m_counter;
		}
	}

	m_pointed = other;
	m_counter = new unsigned int(1);	
	return *this;
}