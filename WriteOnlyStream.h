#pragma once
#include <cstddef>

template <class T>
class WriteOnlyStream {
public:
    virtual ~WriteOnlyStream() = default;
    
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual size_t Write(T item) = 0;
    virtual size_t GetPosition() const = 0;
};