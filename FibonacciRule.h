#pragma once
#include "IGeneratorRule.h"
#include <cstddef>

class FibonacciRule : public IGeneratorRule<size_t> {
public:
    size_t Generate(const Ordinal& index) override {
        size_t n = index.GetFinite();
        if (n == 0) return 0;
        if (n == 1) return 1;
        
        size_t a = 0, b = 1, res = 0;
        for (size_t i = 2; i <= n; ++i) {
            res = a + b;
            a = b;
            b = res;
        }
        return res;
    }
    
    IGeneratorRule<size_t>* Clone() const override {
        return new FibonacciRule();
    }
};