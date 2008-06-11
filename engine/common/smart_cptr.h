namespace std
{
	class smart_cptr
	{
	public:
		smart_cptr() : m_pointed(NULL), m_counter(NULL) {};
		smart_cptr(char* ptr) : m_pointed(ptr), m_counter(new unsigned int(1)) {};
		smart_cptr(const smart_cptr& other) : m_pointed(other.m_pointed), m_counter(other.m_counter)
		{
			if (m_counter) ++(*m_counter);
		}
		~smart_cptr()
		{
			if (m_counter)
			{
				--(*m_counter);
				if (*m_counter == 0)
				{
					delete [] m_pointed;
					delete m_counter;
				}
			}
		}
		smart_cptr& operator= (const smart_cptr& other)
		{
			if (m_pointed)
			{
				if (m_counter)
				{
					--(*m_counter);
					if (*m_counter == 0)
					{
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
		smart_cptr& operator= (char* other)
		{
			if (m_pointed)
			{
				if (m_counter)
				{
					--(*m_counter);
					if (*m_counter == 0)
					{
						delete m_pointed;
						delete m_counter;
					}
				}
			}
			m_pointed = other;
			m_counter = new unsigned int(1);
			return *this;
		}
		bool operator== (const char* other) const
		{
			return strcmp(m_pointed, other) == 0;
		};
		operator bool()
		{
			return m_pointed != 0;
		};
		operator char*()
		{
			return m_pointed;
		};
		operator const char*() const
		{
			return m_pointed;
		};

	private:
		char* m_pointed;
		unsigned int* m_counter;
	};
};