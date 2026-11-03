#pragma once
#include "IGeneratorRule.h"
#include "Ordinal.h"

template <class T> class LazySequence;

template <class T>
class ConcatRule : public IGeneratorRule<T> {
private:
    LazySequence<T>* first;
    LazySequence<T>* second;

public:
    ConcatRule(LazySequence<T>* seq1, LazySequence<T>* seq2) : first(seq1), second(seq2) {}
    
    T Generate(const Ordinal& index) override;
    
    IGeneratorRule<T>* Clone() const override {
        return new ConcatRule<T>(first, second);
    }
};