#pragma once
#include "ReadOnlyStream.h"
#include "LazySequence.h"
#include "Exceptions.h"
#include "WriteOnlyStream.h"
#include <cstddef>

template <class T>
class SequenceStream : public ReadOnlyStream<T> {
private:
    LazySequence<T>* sequence;
    size_t position;
    bool isOpen;

public:
    explicit SequenceStream(LazySequence<T>* seq)
        : sequence(seq), position(0), isOpen(false) {}

    void Open() override {
        isOpen = true;
        position = 0;
    }

    void Close() override {
        isOpen = false;
    }

    bool IsEndOfStream() const override {
        if (!isOpen) throw InvalidArgument("Поток не открыт.");
        
        
        if (sequence->GetOrdinalLength().IsInfinite()) {
            return false;
        }
        
        
        return position >= sequence->GetOrdinalLength().GetFinite();
    }

    T Read() override {
        if (!isOpen) throw InvalidArgument("Поток не открыт.");
        if (IsEndOfStream()) throw IndexOutOfRange("Достигнут конец потока.");
        
        T item = sequence->Get(position);
        position++;
        return item;
    }

    size_t GetPosition() const override {
        return position;
    }

    bool IsCanSeek() const override { return true; }
    bool IsCanGoBack() const override { return true; }

    size_t Seek(size_t index) override {
        if (!sequence->GetOrdinalLength().IsInfinite() && index >= sequence->GetOrdinalLength().GetFinite()) {
            position = sequence->GetOrdinalLength().GetFinite();
        } else {
            position = index;
        }
        return position;
    }
};

template <class T>
class SequenceWriteStream : public WriteOnlyStream<T> {
private:
    LazySequence<T>* sequence;
    size_t position;
    bool isOpen;

public:
    explicit SequenceWriteStream(LazySequence<T>* seq)
        : sequence(seq), position(0), isOpen(false) {}

    void Open() override {
        isOpen = true;
        position = 0;
    }

    void Close() override {
        isOpen = false;
    }

    size_t Write(T item) override {
        if (!isOpen) throw InvalidArgument("Поток не открыт для записи.");
        if (sequence == nullptr) throw InvalidArgument("Нет целевой последовательности.");
        
        sequence->Append(item);
        position++;
        return position;
    }

    size_t GetPosition() const override {
        return position;
    }
};