#pragma once
#include "Ordinal.h"

template <class T>
class IGeneratorRule {
public:
    virtual ~IGeneratorRule() = default;
    virtual T Generate(const Ordinal& index) = 0;
    virtual IGeneratorRule<T>* Clone() const = 0;
};