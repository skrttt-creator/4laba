#pragma once
#include <cstddef>
#include "Exceptions.h"

class Ordinal {
private:
    size_t omega;  
    size_t finite; 

    static size_t CheckedAdd(size_t left, size_t right) {
        size_t res = left + right;
        if (res < left) { 
            throw InvalidArgument("Переполнение ординала.");
        }
        return res;
    }

public:
    Ordinal() : omega(0), finite(0) {}
    explicit Ordinal(size_t f) : omega(0), finite(f) {}
    Ordinal(size_t o, size_t f) : omega(o), finite(f) {}

    static Ordinal Finite(size_t value) { return Ordinal(0, value); }
    static Ordinal Omega() { return Ordinal(1, 0); }

    size_t GetOmega() const { return omega; }
    size_t GetFinite() const { return finite; }

    bool IsFinite() const { return omega == 0; }
    bool IsInfinite() const { return omega > 0; }
    bool IsZero() const { return omega == 0 && finite == 0; }

    size_t Value() const {
        if (!IsFinite()) {
            throw InvalidArgument("Невозможно получить конечное значение от бесконечного ординала.");
        }
        return finite;
    }

    Ordinal Successor() const {
        return Ordinal(omega, CheckedAdd(finite, 1));
    }

    Ordinal Predecessor() const {
        if (finite == 0) {
            throw InvalidArgument("Невозможно получить предшественника для предельного ординала или нуля.");
        }
        return Ordinal(omega, finite - 1);
    }

    Ordinal operator+(const Ordinal& other) const {
        if (other.omega == 0) {
            return Ordinal(omega, CheckedAdd(finite, other.finite));
        }
        return Ordinal(CheckedAdd(omega, other.omega), other.finite);
    }

    Ordinal operator-(const Ordinal& other) const {
        if (*this < other) {
            throw IndexOutOfRange("Результат вычитания ординалов не может быть отрицательным.");
        }
        if (this->omega > other.omega) {
            return Ordinal(this->omega - other.omega, this->finite);
        }
        return Ordinal(0, this->finite - other.finite);
    }

    bool operator==(const Ordinal& other) const { return omega == other.omega && finite == other.finite; }
    bool operator!=(const Ordinal& other) const { return !(*this == other); }
    
    bool operator<(const Ordinal& other) const {
        if (omega != other.omega) return omega < other.omega;
        return finite < other.finite;
    }
    
    bool operator<=(const Ordinal& other) const { return *this < other || *this == other; }
    bool operator>(const Ordinal& other) const { return other < *this; }
    bool operator>=(const Ordinal& other) const { return other <= *this; }
};