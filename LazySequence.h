#pragma once
#include "DynamicArray.h"
#include "Ordinal.h"
#include "Exceptions.h"
#include "IGeneratorRule.h"
#include "MutationRules.h"
#include "ConcatRule.h"
#include "Decorators.h"

template <class T>
class LazySequence {
private:
    DynamicArray<DynamicArray<T>*> cache;
    IGeneratorRule<T>* rule;
    Ordinal length;

    void InvalidateCache() {
        for (size_t i = 0; i < cache.GetSize(); ++i) {
            auto* inner = cache.Get(i);
            if (inner) delete inner;
        }
        cache = DynamicArray<DynamicArray<T>*>(0);
    }

public:
    explicit LazySequence(IGeneratorRule<T>* genRule = nullptr)
        : cache(0), rule(genRule), length(Ordinal::Omega()) {}

    LazySequence(IGeneratorRule<T>* genRule, Ordinal len)
        : cache(0), rule(genRule), length(len) {}

    ~LazySequence() {
        if (rule) delete rule;
        for (size_t i = 0; i < cache.GetSize(); ++i) {
            auto* inner = cache.Get(i);
            if (inner) delete inner;
        }
    }

    Ordinal GetOrdinalLength() const {
        return length;
    }

    size_t GetLength() const {
        if (length >= Ordinal::Omega()) {
            throw IndexOutOfRange("Попытка получить конечную длину от бесконечной ленивой последовательности");
        }
        return length.GetFinite();
    }

    T Get(size_t index) {
        return GetByOrdinal(Ordinal(index));
    }

    T GetFirst() { 
        return GetByOrdinal(Ordinal(0)); 
    }

    T operator[](size_t index) {
        return Get(index);
    }

    T GetLast() {
        if (length.IsZero()) throw IndexOutOfRange("Последовательность пуста");
        if (length.IsInfinite()) throw InvalidArgument("Строгая бесконечность не имеет последнего элемента");
        return GetByOrdinal(Ordinal(length.GetOmega(), length.GetFinite() - 1));
    }

    void Append(T item) {
        this->rule = new AppendRule<T>(this->rule, item, this->length);
        this->length = Ordinal(length.GetOmega(), length.GetFinite() + 1);
    }

    void Prepend(T item) {
        this->rule = new PrependRule<T>(this->rule, item);
        this->length = Ordinal(length.GetOmega(), length.GetFinite() + 1);
        InvalidateCache();
    }

    void InsertAt(T item, size_t index) {
        Ordinal target(index);
        if (target == length) { Append(item); return; }
        if (target > length) throw IndexOutOfRange("Индекс за пределами последовательности");
        if (index == 0) { Prepend(item); return; }

        this->rule = new InsertAtRule<T>(this->rule, target, item);
        this->length = Ordinal(length.GetOmega(), length.GetFinite() + 1);
        InvalidateCache();
    }

    void RemoveLast()  {
    if (length.GetFinite() == 0 && length.GetOmega() == 0) {
        throw InvalidArgument("Последовательность пуста");
    }

    if (length.GetFinite() == 0 && length.GetOmega() > 0) {
        throw InvalidArgument("Невозможно удалить последний элемент (у бесконечности его нет)");
    }

    length = Ordinal(length.GetOmega(), length.GetFinite() - 1);

    InvalidateCache();
}
    LazySequence<T>* Concat(LazySequence<T>* other) {
        if (!other) throw InvalidArgument("Передан пустой указатель");
        auto* concatRule = new ConcatRule<T>(this, other);
        return new LazySequence<T>(concatRule, this->length + other->length);
    }

    template <class TOut>
    LazySequence<TOut>* Map(IMapFunction<T, TOut>* mapper) {
        auto* mapRule = new MapRule<T, TOut>(this, mapper);
        return new LazySequence<TOut>(mapRule, this->length);
    }

    LazySequence<T>* Where(IWhereFunction<T>* whereFunc) {
        auto* whereRule = new WhereRule<T>(this, whereFunc);
        return new LazySequence<T>(whereRule);
    }

    T Reduce(IReduceFunction<T>* reduceFunc, T initialValue, size_t countToReduce) {
        T result = initialValue;
        for (size_t i = 0; i < countToReduce; ++i) {
            result = reduceFunc->Combine(result, this->Get(i));
        }
        return result;
    }

    size_t GetMaterializedCount() const {
        size_t total = 0;
        for (size_t i = 0; i < cache.GetSize(); ++i) {
            auto* inner = cache.Get(i);
            if (inner) {
                total += inner->GetSize();
            }
        }
        return total;
    }

    T GetByOrdinal(const Ordinal& index) {
        if (index >= length) {
            throw IndexOutOfRange("Выход за пределы длины (конечной или трансфинитной)");
        }

        size_t omegaIdx = index.GetOmega();
        size_t finiteIdx = index.GetFinite();

        if (cache.GetSize() <= omegaIdx) {
            size_t oldSize = cache.GetSize();
            cache.Resize(omegaIdx + 1);
            for (size_t i = oldSize; i <= omegaIdx; ++i) {
                cache.Set(i, nullptr);
            }
        }

        if (cache.Get(omegaIdx) == nullptr) {
            cache.Set(omegaIdx, new DynamicArray<T>(0));
        }

        DynamicArray<T>* innerCache = cache.Get(omegaIdx);

        while (innerCache->GetSize() <= finiteIdx) {
            size_t currentSize = innerCache->GetSize();
            Ordinal currentOrdinal(omegaIdx, currentSize);
            
            T generatedItem = rule ? rule->Generate(currentOrdinal) : T();
            
            innerCache->Resize(currentSize + 1);
            innerCache->Set(currentSize, generatedItem);
        }

        return innerCache->Get(finiteIdx);
    }
};


template <class T>
T ConcatRule<T>::Generate(const Ordinal& index) {
    if (index < first->GetOrdinalLength()) {
        return first->GetByOrdinal(index);
    }
    return second->GetByOrdinal(index - first->GetOrdinalLength());
}

template <class T, class TOut>
TOut MapRule<T, TOut>::Generate(const Ordinal& index) {
    T originalItem = source->GetByOrdinal(index);
    return mapper->Map(originalItem);
}

template <class T>
T WhereRule<T>::Generate(const Ordinal&) {
    while (true) {
        T item = source->GetByOrdinal(currentSourceIndex);
        currentSourceIndex = currentSourceIndex + Ordinal(1);
        if (filter->Check(item)) {
            return item;
        }
    }
}