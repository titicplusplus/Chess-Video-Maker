#include <mutex>
#pragma once

template <class T>
class MultiAccess {
	public:
		MultiAccess(T &variableAccess);
		T &get();
		~MultiAccess();
	private:
		T &m_multiAccess;
		std::mutex m_access;
};

template <class T>
inline MultiAccess<T>::MultiAccess(T &variableAccess) : m_multiAccess {variableAccess} {}

template <class T>
inline T &MultiAccess<T>::get() {
	std::lock_guard<std::mutex> lck {this->m_access};
	return this->m_multiAccess;
}

template <class T>
inline MultiAccess<T>::~MultiAccess() {}

