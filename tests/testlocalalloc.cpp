
#include <scoped_allocator>
#include <string>
#include <string_view>
#include <vector>
#include "doctest/doctest.h"
#include "sym2/expr.h"
#include "sym2/allocator.h"

using namespace sym2;

TEST_CASE("LocalAlloc")
{
    // These tests use terrible UB hacks to implement their assertions. When it turns out that they
    // tend to break on some platform/configuration, we should probably replace them by something
    // nicer. Until then, let's be pragmatic and happy with having some test coverage, even if ugly.
    SUBCASE("Simple, non-nested case")
    {
        StackBuffer<32> arena;
        std::vector<char, LocalAlloc<char>> v{&arena};

        v.reserve(10);

        for (int i = 0; i < 10; ++i) {
            v.push_back(static_cast<char>('a' + i));
        }

        const std::string_view inspect{*reinterpret_cast<char**>(&arena), 10};
        CHECK(inspect == "abcdefghij");
    }

    SUBCASE("Single-leve nested")
    {
        StackBuffer<32> arena;
        StackBuffer<16> other{&arena};
        std::vector<char, LocalAlloc<char>> v{&other};

        v.reserve(16);

        for (int i = 0; i < 16; ++i) {
            v.push_back(static_cast<char>('a' + i));
        }

        std::string_view inspect{*reinterpret_cast<char**>(&other), 16};
        CHECK(inspect == "abcdefghijklmnop");

        v.reserve(17);
        inspect = {*reinterpret_cast<char**>(&arena), 16};
        CHECK(inspect == "abcdefghijklmnop");
    }

    SUBCASE("Nested allocating types")
    {
        StackBuffer<128> arena;
        using String = std::basic_string<char, std::char_traits<char>, LocalAlloc<char>>;
        std::vector<String, std::scoped_allocator_adaptor<LocalAlloc<String>>> v{&arena};

        v.emplace_back("abcdefghijklmnopqrstuvwxyz");

        const std::string_view inspect = {*reinterpret_cast<char**>(&arena), 128};
        CHECK(inspect.find("abcdefghijklmnopqrstuvwxyz") != std::string_view::npos);
    }

    SUBCASE("Use operator new/delete when no buffer is given")
    {
        std::vector<int, LocalAlloc<int>> v{nullptr};

        // Make sure this doesn't throw, that's all:
        v.push_back(42);
    }

    SUBCASE("Fallback to operator new/delete")
    {
        StackBuffer<16, alignof(std::uint64_t)> arena;
        std::vector<std::uint64_t, LocalAlloc<std::uint64_t>> v{&arena};

        v.reserve(2);

        v.push_back(0);
        CHECK(*reinterpret_cast<std::uint64_t**>(&arena) == v.data());
        v.push_back(1);
        CHECK(*reinterpret_cast<std::uint64_t**>(&arena) == v.data());

        v.push_back(2);
        CHECK(*reinterpret_cast<std::uint64_t**>(&arena) != v.data());
    }

    SUBCASE("Fallback to operator new/delete")
    {
        StackBuffer<4, alignof(int)> arena{nullptr, false};
        std::vector<int, LocalAlloc<int>> v{&arena};

        CHECK_THROWS(v.reserve(2));
    }

    SUBCASE("Rebinding byte to integral")
    {
        StackBuffer<16, alignof(std::max_align_t)> arena{nullptr, false};
        LocalAlloc<> allocator{&arena};

        std::vector<int, LocalAlloc<int>> v{allocator};

        v.reserve(4);

        v.push_back(42);
        v.push_back(43);
        v.push_back(44);
        v.push_back(45);

        CHECK(*reinterpret_cast<int**>(&arena) == v.data());
    }

    SUBCASE("Rebinding integral to byte")
    {
        StackBuffer<16, alignof(std::max_align_t)> arena{nullptr, false};
        LocalAlloc<int> allocator{&arena};

        std::vector<char, LocalAlloc<char>> v{allocator};

        v.reserve(4);

        v.push_back('a');
        v.push_back('b');
        v.push_back('c');
        v.push_back('d');

        const std::string_view inspect{*reinterpret_cast<char**>(&arena), 4};
        CHECK(inspect == "abcd");
    }

    SUBCASE("Scoped allocator for nested container")
    {
        StackBuffer<128> arena{nullptr, false};
        ScopedLocalVec<Expr> v{&arena};

        v.reserve(3);

        v.emplace_back("a");
        v.emplace_back("b");

        CHECK_THROWS_AS(v.emplace_back("c"), std::bad_alloc);
    }

    SUBCASE("Scoped allocator for nested Expr move- and copy-construct")
    {
        StackBuffer<128> arena{nullptr, false};
        ScopedLocalVec<Expr> v{&arena};

        v.reserve(3);

        v.emplace_back("a");
        v.emplace_back("b");

        // This doesn't throw because the allocators refer to the same buffer/arena and there is no
        // need to copy anything.
        ScopedLocalVec<Expr> other{std::move(v)};

        CHECK_THROWS_AS((ScopedLocalVec<Expr>{other, &arena}), std::bad_alloc);
    }
}
