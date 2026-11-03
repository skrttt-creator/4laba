#pragma once
#include "Exceptions.h"
#include "IEnumerator.h"
#include "IEnumerable.h"
#include <cstddef>

template <class T>
class DynamicArray : public IEnumerable<T> {
private:
    T* data;
    size_t size;
    size_t capacity;

    class DynamicArrayEnumerator : public IEnumerator<T> {
    private:
        const DynamicArray<T>* array;
        size_t currentIndex;
        bool started;
    public:
        explicit DynamicArrayEnumerator(const DynamicArray<T>* arr);
        bool MoveNext() override;
        T GetCurrent() const override;
        void Reset() override;
    };

public:
    explicit DynamicArray(size_t size);
    DynamicArray(const T* items, size_t count);
    DynamicArray(const DynamicArray<T>& other);
    DynamicArray<T>& operator=(const DynamicArray<T>& other);
    ~DynamicArray();

    T Get(size_t index) const;
    size_t GetSize() const;
    void Set(size_t index, const T& value);
    void Resize(size_t newSize);
    T& GetRef(size_t index);

    DynamicArray<T>* GetSubArray(size_t startIndex, size_t endIndex) const;

    IEnumerator<T>* GetEnumerator() const override;
};

#include "DynamicArray.tpp"