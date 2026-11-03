#pragma once
#include "DynamicArray.h"

template <class T>
DynamicArray<T>::DynamicArray(size_t size)
    : size(size), capacity(size > 0 ? size : 1) {
    data = new T[capacity];
}

template <class T>
DynamicArray<T>::DynamicArray(const T* items, size_t count)
    : size(count), capacity(count > 0 ? count : 1) {
    data = new T[capacity];
    for (size_t i = 0; i < count; ++i) {
        data[i] = items[i];
    }
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& other)
    : size(other.size), capacity(other.capacity) {
    data = new T[capacity];
    for (size_t i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

template <class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& other) {
    if (this != &other) {
        delete[] data;
        size = other.size;
        capacity = other.capacity;
        data = new T[capacity];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] data;
}

template <class T>
T DynamicArray<T>::Get(size_t index) const {
    if (index >= size) {
        throw IndexOutOfRange("Ошибка индекса массива");
    }
    return data[index];
}

template <class T>
T& DynamicArray<T>::GetRef(size_t index) {
    if (index >= size) {
        throw IndexOutOfRange("Ошибка индекса массива");
    }
    return data[index];
}

template <class T>
size_t DynamicArray<T>::GetSize() const {
    return size;
}

template <class T>
void DynamicArray<T>::Set(size_t index, const T& value) {
    if (index >= size) {
        throw IndexOutOfRange("Ошибка индекса массива");
    }
    data[index] = value;
}

template <class T>
void DynamicArray<T>::Resize(size_t newSize) {
    if (newSize > capacity) {
        size_t newCap = newSize * 2;
        T* newData = new T[newCap];

        for (size_t i = 0; i < size; ++i) {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity = newCap;
    }
    size = newSize;
}

template <class T>
DynamicArray<T>* DynamicArray<T>::GetSubArray(size_t startIndex, size_t endIndex) const {
    if (startIndex >= size || endIndex >= size || startIndex > endIndex) {
        throw IndexOutOfRange("Ошибка границ подмассива");
    }

    size_t newSize = endIndex - startIndex + 1;
    DynamicArray<T>* subArray = new DynamicArray<T>(newSize);

    for (size_t i = 0; i < newSize; ++i) {
        subArray->Set(i, this->Get(startIndex + i));
    }

    return subArray;
}

template <class T>
IEnumerator<T>* DynamicArray<T>::GetEnumerator() const {
    return new DynamicArrayEnumerator(this);
}

template <class T>
DynamicArray<T>::DynamicArrayEnumerator::DynamicArrayEnumerator(const DynamicArray<T>* arr)
    : array(arr), currentIndex(0), started(false) {
}

template <class T>
bool DynamicArray<T>::DynamicArrayEnumerator::MoveNext() {
    if (!started) {
        if (array->GetSize() > 0) {
            started = true;
            return true;
        }
        return false;
    }
    
    if (currentIndex + 1 < array->GetSize()) {
        currentIndex++;
        return true;
    }
    return false;
}

template <class T>
T DynamicArray<T>::DynamicArrayEnumerator::GetCurrent() const {
    if (!started || currentIndex >= array->GetSize()) {
        throw IndexOutOfRange("Итератор вне границ");
    }
    return array->Get(currentIndex);
}

template <class T>
void DynamicArray<T>::DynamicArrayEnumerator::Reset() {
    currentIndex = 0;
    started = false;
}