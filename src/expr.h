#pragma once

#include <boost/static_string/static_string.hpp>
#include <boost/variant2/variant.hpp>
#include <cstdint>
#include <initializer_list>
#include <span>
#include "number.h"
#include "smallvec.h"

namespace sym2 {
    constexpr inline std::size_t maxSymbolNameSize = 10;
    constexpr inline std::uint32_t staticLeafBufferSize = 4;
    constexpr inline std::uint32_t staticStructureBufferSize = 4;

    using String = boost::static_string<maxSymbolNameSize>;
    using LeafUnion = boost::variant2::variant<Number, String>;

    enum class Tag : std::uint32_t { scalar = 1, sum = 1 << 1, product = 1 << 2, power = 1 << 3, function = 1 << 4 };

    Tag operator|(Tag lhs, Tag rhs);
    Tag& operator|=(Tag& lhs, Tag rhs);
    Tag operator&(Tag lhs, Tag rhs);
    Tag& operator&=(Tag& lhs, Tag rhs);

    struct OpDesc {
        Tag info;
        /* For composite types, the number of children - for scalar types, the number of scalars
         * within the parent (>= 1): */
        std::uint32_t count;
    };

    struct ExprView;

    class Expr {
      public:
        Expr(int n);
        Expr(double n);
        explicit Expr(const Number& n);
        explicit Expr(const char* symbol);
        explicit Expr(const String& symbol);
        explicit Expr(const Number& num, const Number& denom);
        explicit Expr(ExprView e);
        Expr(Tag info, std::span<const ExprView> ops);
        Expr(Tag info, std::initializer_list<ExprView> ops);
        Expr(Tag info, SmallVecBase<Expr>&& ops);

        operator ExprView() const;

      private:
        SmallVec<OpDesc, staticStructureBufferSize> structure;
        SmallVec<LeafUnion, staticLeafBufferSize> leaves;
    };

    struct ExprView {
        std::span<const OpDesc> structure;
        std::span<const LeafUnion> leaves;
    };

    bool operator==(ExprView lhs, ExprView rhs);
    bool operator==(ExprView lhs, int rhs);
    bool operator==(int lhs, ExprView rhs);
    bool operator!=(ExprView lhs, ExprView rhs);
    bool operator!=(ExprView lhs, int rhs);
    bool operator!=(int lhs, ExprView rhs);
}
