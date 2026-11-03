#pragma once
#include "IEnumerator.h"

template <class T>
class IEnumerable {
public:
    virtual ~IEnumerable() = default;
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};