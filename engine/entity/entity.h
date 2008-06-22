#pragma once

#include "script/script.h"

namespace component
{
	class Component;
}

namespace entity
{
	class EntityManager;

	class Entity : public script::ScriptedObject
	{
	protected:
		// typedefs
		typedef map<string, shared_ptr<component::Component>> component_map;

	public:
		// typedefs
		typedef component_map::const_iterator component_iterator;

		// constructor/destructor
		Entity(EntityManager* manager, const string& name);
		virtual ~Entity();

		// common entity functions
		virtual const string& getName() { return m_name; }
		virtual EntityManager* getManager() { return m_manager; }

		// component functions
		template<typename T>
		T* createComponent(const string& name, const typename T::desc_type& desc, T** component)
		{
			if(getComponent(name) != NULL)
			{
				INFO("WARNING: tried to create duplicate component \"%s\" on entity \"%s\"", name.c_str(), this->m_name.c_str());
				return NULL;
			}

			shared_ptr<T> ptr(new T(this, name, desc));
			addComponent(name, ptr);
			*component = ptr.get();
			return ptr.get();
		}

		virtual void addComponent(const string& name, shared_ptr<component::Component> component);
		virtual component::Component* getComponent(const string& name);
		template<typename T>
		T* getComponent(const string& name)
		{
			return dynamic_cast<T*>(getComponent(name));
		}
		virtual void removeComponent(const string& name);
		virtual int getComponentCount() { return m_components.size(); }

		// component shortcuts
		virtual component::Component* getPositionComponent() { return m_position; } // shortcut for getComponent("pos")

		// acquire/release methods
		virtual void acquire();
		virtual void release();

		// test methods
		void test1(void)
		{
			INFO("test1 called");
		}

		string test2()
		{
			return string("test2 string");
		}

		const D3DXVECTOR3& test3()
		{
			static D3DXVECTOR3 bleh(1, 2, 3);
			return bleh;
		}

		void test4(string test)
		{
			INFO("test 4 called with string \"%s\"", test.c_str());
		}

		string test5(const string& test)
		{
			INFO("test 5 called with string \"%s\"", test.c_str());
			return test + " was called";
		}

		void test6(int a, float b, const string& c, D3DXVECTOR3 d)
		{
			INFO("test6 called:");
			INFO("	int a = %i", a);
			INFO("  float b = %f", b);
			INFO("  string c = %s", c.c_str());
			INFO("  vector d = (%f, %f, %f)", d.x, d.y, d.z);
		}

		float test7(int a, float b, const string& c, D3DXVECTOR3 d)
		{
			INFO("test6 called:");
			INFO("	int a = %i", a);
			INFO("  float b = %f", b);
			INFO("  string c = %s", c.c_str());
			INFO("  vector d = (%f, %f, %f)", d.x, d.y, d.z);

			return 12.6f;
		}

		// component iterators
		virtual component_iterator begin() { return m_components.begin(); }
		virtual component_iterator end() { return m_components.end(); }

		// script class
		static ScriptedObject::ScriptClass m_scriptClass;

	protected:
		// script functions
		JSObject* createScriptObject();
		void destroyScriptObject();

		// members
		string m_name;
		EntityManager* m_manager;
		component_map m_components;
		component::Component* m_position;
		bool m_acquired;
	};
}