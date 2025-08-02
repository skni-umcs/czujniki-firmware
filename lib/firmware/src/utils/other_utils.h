#include <set>
#ifndef OTHERUTILS_H
#define OTHERUTILS_H

template<typename T>
bool setContains(std::set<T> set, T element) {
	return set.find(element) != set.end();
}

template<typename T>
bool vectorContains(const std::vector<T>& vec, const T& element) {
    return std::find(vec.begin(), vec.end(), element) != vec.end();
}

template<typename T>
void vectorErase(std::vector<T>& vec, const T& element) {
    vec.erase(std::remove(vec.begin(), vec.end(), element), vec.end());
}

#endif