#pragma once

#include <stdexcept>

namespace mylist
{

class ListOutOfRangeException : public std::logic_error
{
public:
    ListOutOfRangeException(const char* msg) noexcept : std::logic_error(msg) {}

    auto what() const noexcept -> const char* override
    {
        return std::logic_error::what();
    }
};

class DanglingIteratorException : public std::runtime_error
{
public:
    DanglingIteratorException(const char* msg) noexcept : std::runtime_error(msg) {}

    auto what() const noexcept -> const char* override
    {
        return std::runtime_error::what();
    }
};

class MovedSelfAppendException : public std::logic_error
{
public:
    MovedSelfAppendException(const char* msg) noexcept : std::logic_error(msg) {}

    auto what() const noexcept -> const char* override
    {
        return std::logic_error::what();
    }
};

} // namespace mylist
