#pragma once

#include <memory>
#include <utility>

namespace mylist
{

template<typename T>
class Node : std::enable_shared_from_this<Node<T>>
{
public:
    T value{};
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;

    static auto create(std::convertible_to<T> auto&& value) -> std::shared_ptr<Node>
    {
        return std::shared_ptr<Node>(new Node(std::forward<decltype(value)>(value)));
    }

    static auto create() -> std::shared_ptr<Node>
    {
        return std::shared_ptr<Node>(new Node());
    }

private:
    Node() = default;
    Node(const T& value) : value(value) {}
    Node(T&& value) : value(std::move(value)) {}
};

} // namespace mylist
