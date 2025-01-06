#include <set>
#ifndef OTHERUTILS_H
#define OTHERUTILS_H

template<typename T>
bool setContains(std::set<T> set, T element) {
	return set.find(element) != set.end();
}

#endif