#pragma once
#include "IGeneratorRule.h"
#include "Ordinal.h"
#include "Exceptions.h"


template <class T> class LazySequence;

template <class T>
class AppendRule : public IGeneratorRule<T> {
private:
    IGeneratorRule<T>* baseRule;
    T newItem;
    Ordinal appendIndex;

public:
    AppendRule(IGeneratorRule<T>* base, T item, Ordinal index)
        : baseRule(base), newItem(item), appendIndex(index) {}
        
    ~AppendRule() override { delete baseRule; }
    
    T Generate(const Ordinal& index) override {
        if (index == appendIndex) return newItem;
        if (baseRule) return baseRule->Generate(index);
        throw IndexOutOfRange("Базовое правило отсутствует");
    }
    
    IGeneratorRule<T>* Clone() const override {
        return new AppendRule<T>(baseRule ? baseRule->Clone() : nullptr, newItem, appendIndex);
    }
};

template <class T>
class PrependRule : public IGeneratorRule<T> {
private:
    IGeneratorRule<T>* baseRule;
    T newItem;

public:
    PrependRule(IGeneratorRule<T>* base, T item) : baseRule(base), newItem(item) {}
    
    ~PrependRule() override { delete baseRule; }
    
    T Generate(const Ordinal& index) override {
        if (index.IsZero()) return newItem;
        if (baseRule) return baseRule->Generate(index.Predecessor());
        throw IndexOutOfRange("Базовое правило отсутствует");
    }
    
    IGeneratorRule<T>* Clone() const override {
        return new PrependRule<T>(baseRule ? baseRule->Clone() : nullptr, newItem);
    }
};

template <class T>
class InsertAtRule : public IGeneratorRule<T> {
private:
    IGeneratorRule<T>* baseRule;
    Ordinal targetIndex;
    T newItem;

public:
    InsertAtRule(IGeneratorRule<T>* base, Ordinal target, T item)
        : baseRule(base), targetIndex(target), newItem(item) {}
        
    ~InsertAtRule() override { delete baseRule; }
    
    T Generate(const Ordinal& index) override {
        if (index < targetIndex) {
            if (baseRule) return baseRule->Generate(index);
            throw IndexOutOfRange("Базовое правило отсутствует");
        }
        if (index == targetIndex) return newItem;
        
        if (baseRule) return baseRule->Generate(index.Predecessor());
        throw IndexOutOfRange("Базовое правило отсутствует");
    }
    
    IGeneratorRule<T>* Clone() const override {
        return new InsertAtRule<T>(baseRule ? baseRule->Clone() : nullptr, targetIndex, newItem);
    }
};