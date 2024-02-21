#pragma once

#include <variant>

template <typename T, typename E>
class Result;

template <typename T>
class Ok {
public:
    Ok(T& value): value(value) {}
    Ok(T&& value): value(std::move(value)) {}

    template <typename E>
    operator Result<T, E>() {
        return Result<T, E>(std::move(value));
    }
private:
    T&& value;
};

template <typename E>
class Error {
public:
    Error(E& value): value(value) {}
    Error(E&& value): value(std::move(value)) {}

    template <typename T>
    operator Result<T, E>() {
        return Result<T, E>(std::move(value));
    }
private:
    E&& value;
};


/*
 * Lightweight wrapper around std::variant for better error reporting
 * and variants handling
 */
template <typename T, typename E>
class Result {
    friend class Ok<T>;
    friend class Error<E>;
public:
    Result() = delete;

    inline T&& unwrap() {
        if (is_ok()) {
            return std::move(std::get<T>(value));
        }
        throw std::get<E>(value);
    }

    inline bool is_ok() {
        return std::holds_alternative<T>(value);
    }

    inline bool is_error() {
        return std::holds_alternative<E>(value);
    }
private:
    Result(T& value): value(value) {}
    Result(T&& value): value(std::move(value)) {}

    Result(E& value): value(value) {}
    Result(E&& value): value(std::move(value)) {}

    std::variant<T, E> value;
};
