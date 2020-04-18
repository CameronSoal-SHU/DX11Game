#pragma once
#include <cassert>

// Reusable Singleton class to be inherited
template<class T> class Singleton {
public:
	Singleton() {
		assert(m_ptrSingleton == nullptr);
		m_ptrSingleton = static_cast<T*>(this);
	}

	virtual ~Singleton() {
		assert(m_ptrSingleton != nullptr);
		m_ptrSingleton = nullptr;
	}

	static T& Get() {
		assert(m_ptrSingleton != nullptr);
		
		return *m_ptrSingleton;
	}
private:
	static T* m_ptrSingleton;
	Singleton(Singleton const&)		 = delete;
	void operator=(Singleton const&) = delete;
};

template<typename T> T* Singleton<T>::m_ptrSingleton = 0;