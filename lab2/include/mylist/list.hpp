#pragma once

#include "_iterators.hpp"
#include "_node.hpp"
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <optional>
#include <ostream>
#include <ranges>
#include <utility>

namespace mylist
{

class ListBase
{
public:
    using size_type = std::size_t;

    virtual ~ListBase() = default;

    auto size() const noexcept -> size_type
    {
        return mLen;
    }

    auto empty() const noexcept -> bool
    {
        return mLen == 0;
    }

protected:
    size_type mLen{};
};

template<typename T>
class List : public ListBase
{
public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = ListBase::size_type;

    using iterator = Iterator<value_type>;
    using const_iterator = ConstIterator<value_type>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    ~List() = default;

    List() = default;
    List(std::initializer_list<value_type> list);
    List(const List& that);
    List(size_type count, value_type value = value_type());
    List(List&& that) noexcept;

    template<std::input_iterator It>
    List(It begin, It end)
        requires std::convertible_to<typename std::iterator_traits<It>::value_type, value_type>;

    template<std::ranges::input_range Rng>
    List(const Rng& range)
        requires std::convertible_to<typename std::iterator_traits<std::ranges::iterator_t<Rng>>::value_type,
                                     value_type>;

    auto operator=(const List& that) -> List&;
    auto operator=(List&& that) noexcept -> List&;

    auto peekHead() -> reference;
    auto peekHead() const -> const_reference;
    auto peekTail() -> reference;
    auto peekTail() const -> const_reference;

    auto popHead() noexcept -> std::optional<value_type>;
    auto popTail() noexcept -> std::optional<value_type>;

    void pushHead(std::convertible_to<value_type> auto&& element);
    void pushTail(std::convertible_to<value_type> auto&& element);

    // Псевдоним `pushHead` для совместимости с front_inserter
    void push_front(std::convertible_to<value_type> auto&& element);

    // Псевдоним `pushTail` для совместимости с back_inserter
    void push_back(std::convertible_to<value_type> auto&& element);

    template<typename... Args>
    auto emplaceHead(Args&&... args) -> reference
        requires std::constructible_from<value_type, Args...>;

    template<typename... Args>
    auto emplaceTail(Args&&... args) -> reference
        requires std::constructible_from<value_type, Args...>;

    void insertBefore(const_iterator position, std::convertible_to<value_type> auto&& value);
    void insertAfter(const_iterator position, std::convertible_to<value_type> auto&& value);

    // Псевдоним `insertBefore` для совместимости с inserter
    void insert(const_iterator position, std::convertible_to<value_type> auto&& value);

    template<typename... Args>
    auto emplaceBefore(const_iterator position, Args&&... args) -> iterator
        requires std::constructible_from<value_type, Args...>;

    template<typename... Args>
    auto emplaceAfter(const_iterator position, Args&&... args) -> iterator
        requires std::constructible_from<value_type, Args...>;

    template<typename... Args>
    auto emplace(const_iterator position, Args&&... args) -> iterator
        requires std::constructible_from<value_type, Args...>;

    auto operator+=(const List& that) -> List&;
    auto operator+=(List&& that) -> List&;
    void append(const List& that);
    void append(List&& that);

    template<std::ranges::input_range Rng>
    void append(const Rng& range)
        requires std::convertible_to<typename Rng::value_type, value_type>;

    template<std::input_iterator It>
    void append(It begin, It end)
        requires std::convertible_to<typename std::iterator_traits<It>::value_type, value_type>;

    void clear() noexcept;
    void swap(List& other) noexcept;

    auto begin() noexcept -> iterator
    {
        return iterator(mHead);
    }

    auto begin() const noexcept -> const_iterator
    {
        return cbegin();
    }

    auto end() noexcept -> iterator
    {
        return iterator(mTerminator);
    }

    auto end() const noexcept -> const_iterator
    {
        return cend();
    }

    auto cbegin() const noexcept -> const_iterator
    {
        return const_iterator(mHead);
    }

    auto cend() const noexcept -> const_iterator
    {
        return const_iterator(mTerminator);
    }

    auto rbegin() noexcept -> reverse_iterator
    {
        return reverse_iterator(end());
    }

    auto rend() noexcept -> reverse_iterator
    {
        return reverse_iterator(begin());
    }

    auto crbegin() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cend());
    }

    auto crend() const noexcept -> const_reverse_iterator
    {
        return const_reverse_iterator(cbegin());
    }

private:
    using ValueNode = Node<value_type>;
    std::shared_ptr<ValueNode> mHead{};
    std::weak_ptr<ValueNode> mTail{};
    std::weak_ptr<ValueNode> mTerminator{}; // фиктивная граница

    auto popTerminator() -> std::shared_ptr<ValueNode>;
    void addTerminator(std::shared_ptr<ValueNode>&& sentinel);
    void pushHead(std::shared_ptr<ValueNode>&& node);
    void pushTail(std::shared_ptr<ValueNode>&& node);
    void insertInEmpty(std::shared_ptr<ValueNode>&& node);
    void insertBefore(const_iterator position, std::shared_ptr<ValueNode>&& node);
};

template<typename T>
template<std::input_iterator It>
List<T>::List(It begin, It end)
    requires std::convertible_to<typename std::iterator_traits<It>::value_type, value_type>
{
    std::ranges::for_each(begin, end, [this](const value_type& element) { pushTail(element); });
}

template<typename T>
List<T>::List(size_type count, value_type value)
{
    for (size_type i = 0; i < count; ++i)
    {
        pushTail(value);
    }
}

template<typename T>
List<T>::List(List&& that) noexcept
    : mHead(std::move(that.mHead)), mTail(std::move(that.mTail)), mTerminator(std::move(that.mTerminator))
{
    mLen = std::exchange(that.mLen, 0);
}

template<typename T>
List<T>::List(const List& that) : List(that.begin(), that.end())
{
}

template<typename T>
List<T>::List(std::initializer_list<value_type> list) : List(list.begin(), list.end())
{
}

template<typename T>
template<std::ranges::input_range Rng>
List<T>::List(const Rng& range)
    requires std::convertible_to<typename std::iterator_traits<std::ranges::iterator_t<Rng>>::value_type, value_type>
    : List(std::ranges::begin(range), std::ranges::end(range))
{
}

template<typename T>
auto List<T>::operator=(const List& that) -> List&
{
    if (this != &that)
    {
        List(that).swap(*this);
    }
    return *this;
}

template<typename T>
auto List<T>::operator=(List&& that) noexcept -> List&
{
    if (this != &that)
    {
        mHead = std::move(that.mHead);
        mTail = std::move(that.mTail);
        mTerminator = std::move(that.mTerminator);
        mLen = std::exchange(that.mLen, 0);
    }
    return *this;
}

template<typename T>
void List<T>::pushHead(std::convertible_to<value_type> auto&& element)
{
    pushHead(ValueNode::create(std::forward<decltype(element)>(element)));
}

template<typename T>
void List<T>::push_front(std::convertible_to<value_type> auto&& element)
{
    pushHead(std::forward<decltype(element)>(element));
}

template<typename T>
void List<T>::pushTail(std::convertible_to<value_type> auto&& element)
{
    pushTail(ValueNode::create(std::forward<decltype(element)>(element)));
}

template<typename T>
void List<T>::push_back(std::convertible_to<value_type> auto&& element)
{
    pushTail(std::forward<decltype(element)>(element));
}

template<typename T>
template<typename... Args>
auto List<T>::emplaceHead(Args&&... args) -> reference
    requires std::constructible_from<value_type, Args...>
{
    pushHead(ValueNode::create(value_type(std::forward<Args>(args)...)));
    return mHead->value;
}

template<typename T>
template<typename... Args>
auto List<T>::emplaceTail(Args&&... args) -> reference
    requires std::constructible_from<value_type, Args...>
{
    pushTail(ValueNode::create(value_type(std::forward<Args>(args)...)));
    return mTail.lock()->value;
}

template<typename T>
void List<T>::insertAfter(const_iterator position, std::convertible_to<value_type> auto&& value)
{
    if (position == cend())
    {
        throw ListOutOfRangeException("Couldn't insert after the end of the list");
    }

    insertBefore(++position, ValueNode::create(std::forward<decltype(value)>(value)));
}

template<typename T>
void List<T>::insertBefore(const_iterator position, std::convertible_to<value_type> auto&& value)
{
    insertBefore(position, ValueNode::create(std::forward<decltype(value)>(value)));
}

template<typename T>
void List<T>::insert(const_iterator position, std::convertible_to<value_type> auto&& value)
{
    insertBefore(position, std::forward<decltype(value)>(value));
}

template<typename T>
template<typename... Args>
auto List<T>::emplaceBefore(const_iterator position, Args&&... args) -> iterator
    requires std::constructible_from<value_type, Args...>
{
    insertBefore(position, ValueNode::create(value_type(std::forward<Args>(args)...)));
    return (--position).currentNode;
}

template<typename T>
template<typename... Args>
auto List<T>::emplaceAfter(const_iterator position, Args&&... args) -> iterator
    requires std::constructible_from<value_type, Args...>
{
    if (position == cend())
    {
        throw ListOutOfRangeException("Couldn't insert after the end of the list");
    }

    return emplaceBefore(++position, std::forward<Args>(args)...);
}

template<typename T>
template<typename... Args>
auto List<T>::emplace(const_iterator position, Args&&... args) -> iterator
    requires std::constructible_from<value_type, Args...>
{
    return emplaceBefore(position, std::forward<Args>(args)...);
}

template<typename T>
auto List<T>::operator+=(const List& that) -> List&
{
    this->append(that);
    return *this;
}

template<typename T>
auto List<T>::operator+=(List&& that) -> List&
{
    this->append(std::move(that));
    return *this;
}

template<typename T>
void List<T>::append(List&& that)
{
    if (this == &that)
    {
        throw MovedSelfAppendException("Trying to concatenate moved list with itself");
    }

    if (empty())
    {
        *this = std::move(that);
    }
    else if (!that.empty())
    {
        popTerminator();
        that.mHead->prev = mTail;
        mTail.lock()->next = std::move(that.mHead);
        mTail = std::move(that.mTail);
        mTerminator = std::move(that.mTerminator);

        mLen += that.mLen;
        that.mLen = 0;
    }
}

template<typename T>
void List<T>::append(const List& that)
{
    std::ranges::for_each(that, [this](const value_type& element) { pushTail(element); });
}

template<typename T>
template<std::ranges::input_range Rng>
void List<T>::append(const Rng& range)
    requires std::convertible_to<typename Rng::value_type, value_type>
{
    append(std::ranges::begin(range), std::ranges::end(range));
}

template<typename T>
template<std::input_iterator It>
void List<T>::append(It begin, It end)
    requires std::convertible_to<typename std::iterator_traits<It>::value_type, value_type>
{
    std::ranges::for_each(begin, end, [this](const value_type& element) { pushTail(element); });
}

template<typename T>
void List<T>::clear() noexcept
{
    mHead.reset();
    mTail.reset();
    mTerminator.reset();
    mLen = 0;
}

template<typename T>
void List<T>::swap(List& other) noexcept
{
    std::swap(mHead, other.mHead);
    std::swap(mTail, other.mTail);
    std::swap(mTerminator, other.mTerminator);
    std::swap(mLen, other.mLen);
}

template<typename T>
auto List<T>::peekHead() -> reference
{
    if (empty())
    {
        throw ListOutOfRangeException("peekHead() called on an empty list");
    }

    return mHead->value;
}

template<typename T>
auto List<T>::peekHead() const -> const_reference
{
    if (empty())
    {
        throw ListOutOfRangeException("peekHead() called on an empty list");
    }

    return mHead->value;
}

template<typename T>
auto List<T>::peekTail() -> reference
{
    if (empty())
    {
        throw ListOutOfRangeException("peekTail() called on an empty list");
    }

    return mTail.lock()->value;
}

template<typename T>
auto List<T>::peekTail() const -> const_reference
{
    if (empty())
    {
        throw ListOutOfRangeException("peekTail() called on an empty list");
    }

    return mTail.lock()->value;
}

template<typename T>
void List<T>::insertInEmpty(std::shared_ptr<ValueNode>&& node)
{
    mHead = std::move(node);
    mHead->next = ValueNode::create();
    mHead->next->prev = mHead;
    mTerminator = mHead->next;
    mTail = mHead;
}

template<typename T>
auto List<T>::popTerminator() -> std::shared_ptr<ValueNode>
{
    return std::move(mTail.lock()->next);
}

template<typename T>
void List<T>::addTerminator(std::shared_ptr<ValueNode>&& sentinel)
{
    sentinel->prev = mTail;
    this->mTerminator = sentinel;
    mTail.lock()->next = std::move(sentinel);
}

template<typename T>
void List<T>::pushHead(std::shared_ptr<ValueNode>&& node)
{
    if (empty())
    {
        insertInEmpty(std::move(node));
    }
    else
    {
        mHead->prev = node;
        node->next = std::move(mHead);
        mHead = std::move(node);
    }
    ++mLen;
}

template<typename T>
void List<T>::pushTail(std::shared_ptr<ValueNode>&& node)
{
    if (empty())
    {
        insertInEmpty(std::move(node));
    }
    else
    {
        auto sent = popTerminator();
        node->prev = mTail;
        mTail.lock()->next = std::move(node);
        mTail = mTail.lock()->next;
        addTerminator(std::move(sent));
    }
    ++mLen;
}

template<typename T>
auto List<T>::popHead() noexcept -> std::optional<value_type>
{
    if (empty())
    {
        return {};
    }

    auto data = std::move(mHead->value);
    mHead = std::move(mHead->next);
    --mLen;

    return data;
}

template<typename T>
auto List<T>::popTail() noexcept -> std::optional<value_type>
{
    if (empty())
    {
        return {};
    }

    auto data = std::move(mTail.lock()->value);
    auto sent = popTerminator();
    mTail = mTail.lock()->prev;
    --mLen;

    if (!empty())
    {
        addTerminator(std::move(sent));
    }
    else
    {
        mHead.reset();
    }

    return data;
}

template<typename T>
void List<T>::insertBefore(const_iterator position, std::shared_ptr<ValueNode>&& node)
{
    position.validateIterator();

    if (position == cbegin())
    {
        pushHead(std::move(node));
        return;
    }

    auto currentNode = position.currentNode.lock();
    auto prevNode = currentNode->prev.lock();
    node->prev = prevNode;
    node->next = std::move(prevNode->next);
    currentNode->prev = node;
    prevNode->next = std::move(node);

    ++mLen;
}

template<typename T>
auto operator+(const List<T>& lhs, const List<T>& rhs) -> List<T>
{
    auto newList = List<T>(lhs);
    newList += rhs;
    return newList;
}

template<typename T>
auto operator<<(std::ostream& os, const mylist::List<T> ls) -> std::ostream&
{
    os << "[";
    for (auto separator = ""; const auto& element : ls)
    {
        os << separator << element;
        separator = ", ";
    }
    os << "]";
    return os;
}

} // namespace mylist
