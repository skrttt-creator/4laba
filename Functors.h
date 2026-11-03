#pragma once

template <class T, class TOut>
class IMapFunction {
public:
    virtual ~IMapFunction() = default;
    virtual TOut Map(T item) = 0;
};

template <class T>
class IWhereFunction {
public:
    virtual ~IWhereFunction() = default;
    virtual bool Check(T item) = 0;
};

template <class T>
class IReduceFunction {
public:
    virtual ~IReduceFunction() = default;
    virtual T Combine(T accum, T item) = 0;
};