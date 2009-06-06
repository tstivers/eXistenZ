#pragma once

namespace eXistenZ
{
	class eventprovider
	{
	};

	class eventargs
	{
	};

	class connection_base
	{
	public:
		virtual ~connection_base() {}
	};

	typedef shared_ptr<connection_base> connection;

	template<typename T>
	class event;

	class eventconsumer
	{
		vector<connection> connections_;

	public:
		virtual ~eventconsumer() {}

		void addConnection(const connection& c)
		{
			connections_.push_back(c);
		}

		template<typename T, typename C>
		void connectEvent(event<T>& e, bool(C::*f)(eventprovider*, const typename event<T>::argument_type&))
		{
			e.connect(this, fastdelegate::MakeDelegate((C*)this, f));
		}
	};

	template<typename T>
	class event
	{
	public:

		template<typename T>
		class connection_impl : public connection_base
		{
			friend class event<T>;

			typedef event<T> event_type;
			typedef typename event_type::function_type function_type;

			event_type* e_;
			function_type f_;

			connection_impl(event_type* e, const function_type& f, int priority) :
			  e_(e), f_(f)
			{
				e_->addConnection(priority, this);
			}

			bool call(eventprovider* p, const T& args)
			{
			  return f_(p, args);
			}
			
		public:
			connection_impl() : e_(NULL) {}

			void disconnect()
			{
				if(e_)
					e_->removeConnection(this);
				e_ = NULL;
			}

			~connection_impl() 
			{ 
				if(e_)
					e_->removeConnection(this); 
			}
		};

		typedef function<bool(eventprovider*, const T&)> function_type;
		typedef connection_impl<T> connection_type;
		typedef T argument_type;

	private:
		// multimap instead of bimap because bimaps are incredibly slow to compile
		typedef	multimap<int, connection_type*> connectionmap;
		connectionmap connections_;

		void addConnection(int priority, connection_type* c)
		{
			connections_.insert(connectionmap::value_type(priority, c));
		}

		void removeConnection(connection_type* c)
		{
			connectionmap::iterator it = connections_.begin();
			while(it != connections_.end())
			{
				if(it->second == c)
					it = connections_.erase(it);
				else
					++it;
			}
		}

	public:

		connection connect(const function_type& f, int priority = 0)
		{
			return connection(new connection_type(this, f, priority));
		}

		void connect(eventconsumer* consumer, const function_type& f, int priority = 0)
		{
			consumer->addConnection(connection(new connection_type(this, f, priority)));
		}

		inline bool operator()(eventprovider* p, const T& args)
		{
			for(connectionmap::const_iterator it = connections_.begin(); it != connections_.end(); ++it)
				if(!it->second->call(p, args))
					return false;

			return true;
		}

		~event()
		{
			for(connectionmap::const_iterator it = connections_.begin(); it != connections_.end(); ++it)
				it->second->e_ = NULL;
		}
	};
}