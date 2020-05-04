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
		return *m_ptrSingleton;
	}
private:
	inline static T* m_ptrSingleton  = 0;

	Singleton(const Singleton&)		 = delete;
	void operator=(const Singleton&) = delete;
};

//template<typename T> T* Singleton<T>::m_ptrSingleton = 0;