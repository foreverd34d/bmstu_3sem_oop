#include "mylist/list.hpp"
#include <cctype>
#include <fmt/ostream.h>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

template<typename T>
struct fmt::formatter<mylist::List<T>> : fmt::ostream_formatter {};

namespace rng = std::ranges;
namespace rnv = std::views;

auto main() -> int
{
    std::ios::sync_with_stdio(false);

    const auto strings = mylist::List<std::string>{"first", "second", "third", "fourth", "fifth"};
    fmt::print(std::cout, "Initialized {} from initializer list\n", strings);

    // Ranges support
    auto startsWithF = [](const std::string& str) { return str.starts_with('f'); };
    auto strToUpper = [](const std::string& str) {
        auto copy = str;
        rng::transform(copy, copy.begin(), [](char ch) { return std::toupper(ch); });
        return copy;
    };
    auto modStringsView = strings | rnv::filter(startsWithF) | rnv::transform(strToUpper) | rnv::reverse;
    auto modStrings = mylist::List<std::string>(modStringsView.begin(), modStringsView.end());
    fmt::print(std::cout, "Modified: {}\n", modStrings);

    // Concatenation
    auto concat = strings + modStrings;
    fmt::print(std::cout, "Concatenated: {}\n", concat);

    // Reverse iterators
    auto revConcat = mylist::List<std::string>(concat.rbegin(), concat.rend());

    // Emplace functions
    revConcat.emplaceHead("head");
    revConcat.emplaceTail("tail");
    revConcat.emplace(rng::next(revConcat.begin(), revConcat.size() / 2), "middle");

    // Peek functions
    revConcat.peekHead().append("_");
    revConcat.peekTail().append("_");

    fmt::print(std::cout, "Pushed some strings: {}\n\n", revConcat);

    auto danglingIt = revConcat.begin();
    fmt::print(std::cout, "Got iterator to the beginning, dangling = {}, value = {}\n", danglingIt.dangling(), *danglingIt);

    fmt::print(std::cout, "Popping from head\n");
    while (auto str = revConcat.popHead())
    {
        fmt::print(std::cout, "{:<8}: {}\n", *str, revConcat);
    }

    // Dangling iterators check
    fmt::print(std::cout, "\nDid the iterator to the beginning expire? {}\n", danglingIt.dangling());

    try
    {
        fmt::print(std::cout, "Trying to dereference it\n");
        *danglingIt;
    }
    catch (const mylist::DanglingIteratorException& ex)
    {
        fmt::print(std::cout, "Got exception with message \"{}\"\n", ex.what());
    }
}
