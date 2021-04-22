
#include <boost/range/algorithm.hpp>
#include "autosimpl.h"
#include "doctest/doctest.h"
#include "predicates.h"
#include "testutils.h"
#include "view.h"

using namespace sym2;

template <PredicateTag auto to, PredicateTag auto from>
constexpr bool isImplicit = std::is_convertible_v<ExprView<from>, ExprView<to>>;
template <PredicateTag auto to, PredicateTag auto from>
constexpr bool isExplicit = !isImplicit<to, from>;

static_assert(isExplicit<symbol, number>);
static_assert(isExplicit<number, symbol>);
static_assert(isExplicit < number && realDomain && positive, number >);

static_assert(isExplicit<symbol, any>);
static_assert(isExplicit < sum && positive, any >);

static_assert(isImplicit < any, number || symbol >);
static_assert(isImplicit<any, any>);
static_assert(isImplicit < any, realDomain&& complexDomain&& symbol >);
static_assert(isImplicit<number, number>);
static_assert(isImplicit < number && realDomain && positive, number&& realDomain&& positive >);

static_assert(isImplicit < number, number&& realDomain&& positive >);
static_assert(isImplicit < number && realDomain, number&& realDomain&& positive >);
static_assert(isImplicit < symbol && complexDomain, symbol&& small&& complexDomain >);
static_assert(isExplicit < symbol && complexDomain, symbol&& small&& realDomain >);

static_assert(isImplicit < number || symbol || function, number || function >);
static_assert(isImplicit < number || symbol, number&& symbol && !function >);

TEST_CASE("Basic ExprView behavior")
{
    const auto s = autoSum("a"_ex, "b"_ex);
    const auto sv = view(s);

    SUBCASE("Size")
    {
        CHECK(sv.size() == 3);
    }

    SUBCASE("Copy and compare")
    {
        std::vector<Blob> dest;

        boost::copy(sv, std::back_inserter(dest));

        boost::equal(
          sv, dest, [](const Blob& lhs, const Blob& rhs) { return std::memcmp(&lhs, &rhs, sizeof(Blob)) == 0; });
    }
}
