#include "mylist/list.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

TEST_CASE("List constructors and assignment operators")
{
    namespace rg = std::ranges;
    auto lsToCopy = mylist::List<int>{1, 2, 3, 4, 5};
    auto vecToCopy = std::vector<int>{1, 2, 3, 4, 5};

    SECTION("Default")
    {
        auto ls = mylist::List<int>();
        REQUIRE(ls.empty());
        REQUIRE_THROWS_AS(ls.peekHead(), mylist::ListOutOfRangeException);
    }

    SECTION("Iterator")
    {
        auto ls = mylist::List<int>(vecToCopy.begin(), vecToCopy.end());
        REQUIRE(ls.size() == vecToCopy.size());
        REQUIRE(rg::equal(ls, vecToCopy));
    }

    SECTION("Range")
    {
        auto ls = mylist::List<int>(vecToCopy);
        REQUIRE(ls.size() == vecToCopy.size());
        REQUIRE(rg::equal(ls, vecToCopy));
    }

    SECTION("Initializer list")
    {
        auto ls = mylist::List<int>{1, 2, 3, 4, 5};
        REQUIRE(ls.size() == 5);
        REQUIRE_NOTHROW(ls.peekHead() == 1);
        REQUIRE_NOTHROW(ls.peekTail() == 5);
    }

    SECTION("Fill")
    {
        auto ls = mylist::List<int>(5);
        REQUIRE(ls.size() == 5);
        REQUIRE(rg::all_of(ls, [](int value) { return value == 0; }));
    }

    SECTION("Fill with value")
    {
        auto ls = mylist::List<int>(5, 10);
        REQUIRE(ls.size() == 5);
        REQUIRE(rg::all_of(ls, [](int value) { return value == 10; }));
    }

    SECTION("Copy")
    {
        auto ls = mylist::List<int>(lsToCopy);
        REQUIRE_FALSE(lsToCopy.empty());
        REQUIRE(ls.size() == lsToCopy.size());
        REQUIRE(rg::equal(ls, lsToCopy));
    }

    SECTION("Move")
    {
        auto lsToMove = mylist::List<int>(lsToCopy);
        auto lsToMoveSize = lsToMove.size();
        CHECK_FALSE(lsToMove.empty());

        auto ls = mylist::List<int>(std::move(lsToMove));
        REQUIRE(ls.size() == lsToMoveSize);
        REQUIRE(lsToMove.empty());
        REQUIRE(rg::equal(ls, lsToCopy));
    }

    SECTION("Copy assignment")
    {
        auto ls = lsToCopy;
        REQUIRE_FALSE(lsToCopy.empty());
        REQUIRE(ls.size() == lsToCopy.size());
        REQUIRE(rg::equal(ls, lsToCopy));
    }

    SECTION("Move assignment")
    {
        auto lsToMove = lsToCopy;
        auto lsToMoveSize = lsToMove.size();
        CHECK_FALSE(lsToMove.empty());

        auto ls = std::move(lsToMove);
        REQUIRE(ls.size() == lsToMoveSize);
        REQUIRE(lsToMove.empty());
        REQUIRE(rg::equal(ls, lsToCopy));
    }
}

TEST_CASE("List peek methods")
{
    auto nonEmpty = mylist::List<int>{1, 2, 3, 4, 5};
    auto empty = mylist::List<int>();

    SECTION("peekHead on non-empty list")
    {
        REQUIRE(nonEmpty.peekHead() == 1);
    }

    SECTION("peekTail on non-empty list")
    {
        REQUIRE(nonEmpty.peekTail() == 5);
    }

    SECTION("peekHead on empty list")
    {
        REQUIRE_THROWS_AS(empty.peekHead(), mylist::ListOutOfRangeException);
    }

    SECTION("peekTail on empty list")
    {
        REQUIRE_THROWS_AS(empty.peekTail(), mylist::ListOutOfRangeException);
    }
}

TEST_CASE("List push methods")
{
    auto ls = mylist::List<std::vector<int>>{{1}, {2}};
    auto vecToPush = std::vector<int>{3};
    auto lsOldSize = ls.size();

    CHECK(ls.size() == 2);
    CHECK(ls.peekHead()[0] == 1);
    CHECK(ls.peekTail()[0] == 2);

    SECTION("pushHead copy")
    {
        ls.pushHead(vecToPush);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekHead()[0] == 3);
    }

    SECTION("pushTail copy")
    {
        ls.pushTail(vecToPush);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekTail()[0] == 3);
        REQUIRE_FALSE(vecToPush.empty());
    }

    SECTION("pushHead move")
    {
        ls.pushHead(std::move(vecToPush));
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekHead()[0] == 3);
        REQUIRE(vecToPush.empty());
    }

    SECTION("pushTail move")
    {
        ls.pushTail(std::move(vecToPush));
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekTail()[0] == 3);
        REQUIRE(vecToPush.empty());
    }
}

TEST_CASE("List insert methods")
{
    namespace rg = std::ranges;

    auto ls = mylist::List<int>{1, 2, 3, 4};
    auto numToInsert = 100;
    auto lsOldSize = ls.size();
    auto middle = rg::next(ls.cbegin(), ls.size() / 2);

    CHECK(ls.size() == 4);
    CHECK(ls.peekHead() == 1);

    SECTION("insertBefore the beginning")
    {
        ls.insertBefore(ls.cbegin(), numToInsert);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, numToInsert) == ls.cbegin());
    }

    SECTION("insertBefore the end")
    {
        ls.insertBefore(ls.cend(), numToInsert);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, numToInsert) == std::prev(ls.cend()));
    }

    SECTION("insertBefore the middle")
    {
        ls.insertBefore(middle, numToInsert);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, numToInsert) == rg::prev(middle));
    }

    SECTION("insertAfter the beginning")
    {
        ls.insertAfter(ls.cbegin(), numToInsert);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, numToInsert) == rg::next(ls.cbegin()));
    }

    SECTION("insertAfter the middle")
    {
        ls.insertAfter(middle, numToInsert);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, numToInsert) == rg::next(middle));
    }

    SECTION("insertAfter the end")
    {
        REQUIRE_THROWS_AS(ls.insertAfter(ls.cend(), numToInsert), mylist::ListOutOfRangeException);
    }
}

TEST_CASE("List emplace methods")
{
    namespace rg = std::ranges;

    auto ls = mylist::List<std::pair<int, int>>({
        {1, 3},
        {2, 4}
    });
    auto lsOldSize = ls.size();
    auto middle = rg::next(ls.cbegin(), ls.size() / 2);

    SECTION("emplaceHead")
    {
        ls.emplaceHead(5, 7);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekHead() == std::pair<int, int>(5, 7));
    }

    SECTION("emplaceTail")
    {
        ls.emplaceTail(5, 7);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(ls.peekTail() == std::pair<int, int>(5, 7));
    }

    SECTION("emplaceBefore the middle")
    {
        auto emplacedIt = ls.emplaceBefore(middle, 5, 7);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, std::pair<int, int>(5, 7)) == rg::prev(middle));
        REQUIRE(emplacedIt == rg::prev(middle));
    }

    SECTION("emplaceAfter the middle")
    {
        auto emplacedIt = ls.emplaceAfter(middle, 5, 7);
        REQUIRE(ls.size() == lsOldSize + 1);
        REQUIRE(rg::find(ls, std::pair<int, int>(5, 7)) == rg::next(middle));
        REQUIRE(emplacedIt == rg::next(middle));
    }
}

TEST_CASE("List size and empty methods")
{
    SECTION("Non-empty list")
    {
        auto ls = mylist::List<int>{1, 2, 3};
        REQUIRE_FALSE(ls.empty());
        REQUIRE(ls.size() == 3);
    }

    SECTION("Empty list")
    {
        auto ls = mylist::List<int>();
        REQUIRE(ls.empty());
        REQUIRE(ls.size() == 0);
    }
}

TEST_CASE("List pop methods")
{
    auto ls = mylist::List<int>{1, 2, 3, 4, 5};
    auto lsCopy = ls;

    SECTION("popHead")
    {
        auto firstValue = ls.popHead();
        REQUIRE(ls.size() == lsCopy.size() - 1);
        REQUIRE(firstValue.has_value());
        REQUIRE(*firstValue == lsCopy.peekHead());
    }

    SECTION("popTail")
    {
        auto lastValue = ls.popTail();
        REQUIRE(ls.size() == lsCopy.size() - 1);
        REQUIRE(lastValue.has_value());
        REQUIRE(*lastValue == lsCopy.peekTail());
    }

    SECTION("popHead until empty")
    {
        auto lsCopyIt = lsCopy.begin();
        while (auto num = ls.popHead())
        {
            REQUIRE(num.has_value());
            REQUIRE(*num == *lsCopyIt);
            ++lsCopyIt;
        }
    }

    SECTION("popTail until empty")
    {
        auto lsCopyIt = lsCopy.rbegin();
        while (auto num = ls.popTail())
        {
            REQUIRE(num.has_value());
            REQUIRE(*num == *lsCopyIt);
            ++lsCopyIt;
        }
    }
}

TEST_CASE("List clear method")
{
    auto ls = mylist::List<int>{1, 2, 3, 4, 5};
    CHECK(ls.size() == 5);
    CHECK_FALSE(ls.empty());

    ls.clear();
    REQUIRE(ls.empty());
}

TEST_CASE("List swap method")
{
    auto first = mylist::List<int>{1, 2, 3};
    auto second = mylist::List<int>{4, 5, 6};

    auto firstCopy = first;
    auto secondCopy = second;

    first.swap(second);

    REQUIRE(first.size() == secondCopy.size());
    REQUIRE(second.size() == firstCopy.size());
    REQUIRE(std::ranges::equal(first, secondCopy));
    REQUIRE(std::ranges::equal(second, firstCopy));
}

TEST_CASE("List concatenation methods")
{
    auto odds = mylist::List<int>{1, 3, 5};
    auto evens = mylist::List<int>{2, 4, 6};
    auto vec = std::vector<int>{2, 4, 6};

    auto oddsCopy = odds;
    auto evensCopy = evens;

    auto isMadeOf = [](const mylist::List<int>& cat, const mylist::List<int>& left, const std::ranges::input_range auto& right) {
        auto catIt = cat.begin();

        for (const auto& num : left)
        {
            REQUIRE(num == *(catIt++));
        }

        for (const auto& num : right)
        {
            REQUIRE(num == *(catIt++));
        }

        REQUIRE(catIt == cat.end());
    };

    SECTION("append copy")
    {
        odds.append(evens);
        REQUIRE(odds.size() == oddsCopy.size() + evensCopy.size());
        REQUIRE(evens.size() == evensCopy.size());
        isMadeOf(odds, oddsCopy, evensCopy);
    }

    SECTION("append move")
    {
        odds.append(std::move(evens));
        REQUIRE(odds.size() == oddsCopy.size() + evensCopy.size());
        REQUIRE(evens.empty());
        isMadeOf(odds, oddsCopy, evensCopy);
    }

    SECTION("append iterator range")
    {
        odds.append(vec.begin(), vec.end());
        REQUIRE(odds.size() == oddsCopy.size() + vec.size());
        isMadeOf(odds, oddsCopy, vec);
    }

    SECTION("append iterator range")
    {
        odds.append(vec);
        REQUIRE(odds.size() == oddsCopy.size() + vec.size());
        isMadeOf(odds, oddsCopy, vec);
    }
    
    SECTION("operator+= copy")
    {
        odds += evens;
        REQUIRE(odds.size() == oddsCopy.size() + evensCopy.size());
        REQUIRE(evens.size() == evensCopy.size());
        isMadeOf(odds, oddsCopy, evensCopy);
    }

    SECTION("operator+= move")
    {
        odds += std::move(evens);
        REQUIRE(odds.size() == oddsCopy.size() + evensCopy.size());
        REQUIRE(evens.empty());
        isMadeOf(odds, oddsCopy, evensCopy);
    }

    SECTION("operator+")
    {
        auto cat = odds + evens;
        REQUIRE(cat.size() == odds.size() + evens.size());
        REQUIRE(odds.size() == oddsCopy.size());
        REQUIRE(evens.size() == evensCopy.size());
        isMadeOf(cat, odds, evens);
    }
}

TEST_CASE("List iterators")
{
    auto ls = mylist::List<int>{1, 2, 3, 4, 5};

    static_assert(std::ranges::bidirectional_range<mylist::List<int>>);
    static_assert(std::bidirectional_iterator<mylist::List<int>::iterator>);
    static_assert(std::bidirectional_iterator<mylist::List<int>::const_iterator>);
    static_assert(std::bidirectional_iterator<mylist::List<int>::reverse_iterator>);
    static_assert(std::bidirectional_iterator<mylist::List<int>::const_reverse_iterator>);

    SECTION("begin() and end()")
    {
        auto beginIt = ls.begin();
        REQUIRE(*beginIt == 1);
        REQUIRE_THROWS_AS(*std::ranges::prev(beginIt), mylist::DanglingIteratorException);

        auto endIt = ls.end();
        REQUIRE(*std::ranges::prev(endIt) == 5);
    }

    SECTION("begin() and end() on an empty list")
    {
        auto ls = mylist::List<int>();
        REQUIRE(ls.begin() == ls.end());
    }

    SECTION("rbegin() and rend()")
    {
        auto revBeginIt = ls.rbegin();
        REQUIRE(*revBeginIt == 5);
        REQUIRE_THROWS_AS(*std::ranges::prev(revBeginIt), mylist::DanglingIteratorException);

        auto revEndIt = ls.rend();
        REQUIRE(*std::ranges::prev(revEndIt) == 1);
    }

    SECTION("dangling iterator checking")
    {
        auto it = mylist::List<int>::iterator();

        {
            auto temp = mylist::List<int>{1, 2};
            it = temp.begin();
            REQUIRE_FALSE(it.dangling());
            REQUIRE(*it == 1);
        }

        REQUIRE(it.dangling());
        REQUIRE_THROWS_AS(*it, mylist::DanglingIteratorException);
        REQUIRE_THROWS_AS(++it, mylist::DanglingIteratorException);
        REQUIRE_THROWS_AS(--it, mylist::DanglingIteratorException);
    }
}

TEST_CASE("ListBase")
{
    auto ls = std::shared_ptr<mylist::List<int>>(new mylist::List<int>{1, 2, 3, 4, 5});
    auto lsBase = std::shared_ptr<mylist::ListBase>(ls);

    static_assert(std::is_base_of_v<decltype(lsBase)::element_type, decltype(ls)::element_type>);

    REQUIRE(lsBase->size() == ls->size());
    REQUIRE(lsBase->size() == 5);
    REQUIRE_FALSE(lsBase->empty());
}
