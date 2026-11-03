#pragma once
#include "IGeneratorRule.h"
#include "Functors.h"
#include "Ordinal.h"

template <class T> class LazySequence;

template <class T, class TOut>
class MapRule : public IGeneratorRule<TOut> {
private:
    LazySequence<T>* source;
    IMapFunction<T, TOut>* mapper;

public:
    MapRule(LazySequence<T>* src, IMapFunction<T, TOut>* mapFunc)
        : source(src), mapper(mapFunc) {}
        
    TOut Generate(const Ordinal& index) override;
    
    IGeneratorRule<TOut>* Clone() const override {
        return new MapRule<T, TOut>(source, mapper);
    }
};

template <class T>
class WhereRule : public IGeneratorRule<T> {
private:
    LazySequence<T>* source;
    IWhereFunction<T>* filter;
    Ordinal currentSourceIndex;

public:
    WhereRule(LazySequence<T>* src, IWhereFunction<T>* whereFunc)
        : source(src), filter(whereFunc), currentSourceIndex(0) {}
        
    T Generate(const Ordinal& index) override;
    
    IGeneratorRule<T>* Clone() const override {
        auto* cloned = new WhereRule<T>(source, filter);
        cloned->currentSourceIndex = this->currentSourceIndex;
        return cloned;
    }
};