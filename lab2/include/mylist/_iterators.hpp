#pragma once

#include "_node.hpp"
#include "_exceptions.hpp"
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace mylist
{

template<typename T>
class List;

template<typename T>
class ConstIterator;

template<typename T>
class Iterator
{
    friend class List<T>;
    friend class ConstIterator<T>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator() = default;

    Iterator(std::weak_ptr<Node<value_type>> node) : currentNode(node) {}

    Iterator(std::shared_ptr<Node<value_type>> node) : currentNode(node) {}

    Iterator(const Iterator& that) : currentNode(that.currentNode) {}

    auto operator=(const Iterator& that) -> Iterator&
    {
        if (this != &that)
        {
            Iterator(that).swap(*this);
        }
        return *this;
    }

    void swap(Iterator& that) noexcept
    {
        std::swap(currentNode, that.currentNode);
    }

    auto operator++() -> Iterator&
    {
        validateIterator();
        currentNode = currentNode.lock()->next;
        return *this;
    }

    auto operator++(int) -> Iterator
    {
        auto oldIt = Iterator(currentNode);
        ++(*this);
        return oldIt;
    }

    auto operator--() -> Iterator&
    {
        validateIterator();
        currentNode = currentNode.lock()->prev;
        return *this;
    }

    auto operator--(int) -> Iterator
    {
        auto oldIt = Iterator(currentNode);
        --(*this);
        return oldIt;
    }

    auto operator*() const -> reference
    {
        validateIterator();
        return currentNode.lock()->value;
    }

    auto operator->() const -> pointer
    {
        validateIterator();
        return &currentNode.lock()->value;
    }

    friend auto operator==(const Iterator& lhs, const Iterator& rhs) -> bool
    {
        return lhs.currentNode.lock() == rhs.currentNode.lock();
    }

    friend auto operator!=(const Iterator& lhs, const Iterator& rhs) -> bool
    {
        return !(lhs == rhs);
    }

    auto dangling() const noexcept -> bool
    {
        return currentNode.expired();
    }

private:
    std::weak_ptr<Node<value_type>> currentNode{};

    auto validateIterator() const -> void
    {
        if (currentNode.expired())
        {
            throw DanglingIteratorException("Trying to dereference dangling iterator");
        }
    }
};

template<typename T>
class ConstIterator
{
    friend class List<T>;
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    ConstIterator() = default;

    ConstIterator(std::weak_ptr<Node<value_type>> node) : currentNode(node) {}

    ConstIterator(std::shared_ptr<Node<value_type>> node) : currentNode(node) {}

    ConstIterator(const ConstIterator& that) : currentNode(that.currentNode) {}

    ConstIterator(const Iterator<T>& that) : currentNode(that.currentNode) {}

    auto operator=(const ConstIterator& that) -> ConstIterator&
    {
        if (this != &that)
        {
            ConstIterator(that).swap(*this);
        }
        return *this;
    }

    auto operator=(const Iterator<T>& that) -> ConstIterator&
    {
        if (this != &that)
        {
            ConstIterator(that).swap(*this);
        }
        return *this;
    }

    void swap(ConstIterator& that) noexcept
    {
        std::swap(currentNode, that.currentNode);
    }

    auto operator++() -> ConstIterator&
    {
        validateIterator();
        currentNode = currentNode.lock()->next;
        return *this;
    }

    auto operator++(int) -> ConstIterator
    {
        auto oldIt = ConstIterator(currentNode);
        ++(*this);
        return oldIt;
    }

    auto operator--() -> ConstIterator&
    {
        validateIterator();
        currentNode = currentNode.lock()->prev;
        return *this;
    }

    auto operator--(int) -> ConstIterator
    {
        auto oldIt = ConstIterator(currentNode);
        --(*this);
        return oldIt;
    }

    auto operator*() const -> reference
    {
        validateIterator();
        return currentNode.lock()->value;
    }

    auto operator->() const -> pointer
    {
        validateIterator();
        return &currentNode.lock()->value;
    }

    friend auto operator==(const ConstIterator& lhs, const ConstIterator& rhs) -> bool
    {
        return lhs.currentNode.lock() == rhs.currentNode.lock();
    }

    friend auto operator!=(const ConstIterator& lhs, const ConstIterator& rhs) -> bool
    {
        return !(lhs == rhs);
    }

    auto dangling() const noexcept -> bool
    {
        return currentNode.expired();
    }

private:
    std::weak_ptr<Node<value_type>> currentNode{};

    auto validateIterator() const -> void
    {
        if (currentNode.expired())
        {
            throw DanglingIteratorException("Trying to dereference dangling iterator");
        }
    }
};

} // namespace mylist
