#pragma once
#include <cstddef>

template <class T>
class ReadOnlyStream {
public:
    virtual ~ReadOnlyStream() = default;
    
    virtual bool IsEndOfStream() const = 0;
    virtual T Read() = 0;
    virtual size_t GetPosition() const = 0;
    
    virtual bool IsCanSeek() const = 0;
    virtual size_t Seek(size_t index) = 0;
    virtual bool IsCanGoBack() const = 0;
    
    virtual void Open() = 0;
    virtual void Close() = 0;
};