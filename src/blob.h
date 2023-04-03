#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include "blob.h"
#include "largeint.h"
#include "largerational.h"
#include "sym2/compositetype.h"
#include "sym2/domainflag.h"
#include "sym2/doublefctptr.h"
#include "sym2/allocator.h"
#include "sym2/smallrational.h"

namespace sym2 {
    struct alignas(double) Blob {
        std::array<std::byte, 8> bytes;
    };

    // For choosing the single-Blob symbol construction vs. constructing a sequence.
    bool isSmallName(std::string_view name) noexcept;

    Blob construct(std::int16_t n) noexcept;
    // Always returns the canonical form of a rational number using gcd, and an integer if denom
    // is one. Negative denominator causes both numerator's and denominator's sign to be
    // flipped. UB if denom is zero.
    Blob construct(std::int16_t num, std::int16_t denom) noexcept;
    // Expects a short symbol, where isSmallName(symbolName) returns true (UB otherwise).
    Blob construct(std::string_view symbolName, DomainFlag domain) noexcept;

    // Construction of composite structures that represent leafs. All these throw std::range_error
    // when the number of Blobs to store exceed 2^16-1.
    LocalVec<Blob> constructSequence(double value, LocalAlloc<> alloc);
    // Longer symbols are stored as a header and one or more blobs that contain the string data. No
    // restrictions on the name, and always terminated by a null byte. Don't use this function if
    // isSmallName(symbolName) is true.
    LocalVec<Blob> constructSequence(
      std::string_view symbolName, DomainFlag domain, LocalAlloc<> alloc);
    // Constructs a constant, consisting of a constant header blob followed by a symbol blob
    // (short or large, depending on the name length) and a double blob.
    LocalVec<Blob> constructSequence(std::string_view symbolName, double value, LocalAlloc<> alloc);
    // The large integer is expected to not fit into the small integer type (i.e. callers should
    // check this first, and potentially construct a small integer instead).
    LocalVec<Blob> constructSequence(const LargeInt& n, LocalAlloc<> alloc);
    // One of numerator and denominator can fit into a small integer, in which case it is
    // returned as a small integer, but not both of them (callers should check this case and use
    // a small rational type instead).
    LocalVec<Blob> constructSequence(const LargeRational& n, LocalAlloc<> alloc);
    // Constructs unary and binary functions:
    LocalVec<Blob> constructSequence(
      std::string_view function, const Blob* arg, UnaryDoubleFctPtr eval, LocalAlloc<> allocator);
    LocalVec<Blob> constructSequence(std::string_view function, const Blob* arg1, const Blob* arg2,
      BinaryDoubleFctPtr eval, LocalAlloc<> allocator);

    // Assumes that the follow-up Blobs are placed right after the header blob.
    Blob constructCompositeHeader(
      CompositeType composite, std::uint16_t numOperands, std::uint32_t extent) noexcept;
    // Constructs a duplicate, irrespective of whether the original object is self-contained in a
    // single blob or not.
    LocalVec<Blob> constructDuplicateSequence(const Blob* from, LocalAlloc<> allocator);
    // Appends a duplicate of the given expression. Works for self-contained single blob expressions
    // and those with additional blobs. The second parameter dictates the index at which the first
    // blob of the duplicate is placed in the output container. If the output container is not large
    // enough for the new content, it is resized.
    void appendDuplicateSequence(const Blob* from, std::size_t where, LocalVec<Blob>& output);

    bool isNumberHeader(Blob header) noexcept;
    bool isIntegerHeader(Blob header) noexcept;
    bool isRationalHeader(Blob header) noexcept;
    bool isFloatingPointHeader(Blob header) noexcept;
    bool isComplexNumberHeader(Blob header) noexcept;
    // True for small integer and rationals, false otherwise:
    bool isSmallHeader(Blob header) noexcept;
    // Negation of isSmallHeader:
    bool isLargeHeader(Blob header) noexcept;
    bool isScalarHeader(Blob header) noexcept;
    bool isCompositeHeader(Blob header) noexcept;
    bool isSymbolHeader(Blob header) noexcept;
    bool isConstantHeader(Blob header) noexcept;
    bool isSumHeader(Blob header) noexcept;
    bool isProductHeader(Blob header) noexcept;
    bool isPowerHeader(Blob header) noexcept;
    bool isFunctionHeader(Blob header) noexcept;

    // The remote extent is the number of blobs stored externally, i.e., in addition, to the root
    // header.
    std::uint32_t remoteExtent(const Blob* header) noexcept;
    // Returns the number of logical operands: zero for scalars, number of function arguments for
    // functions, the number of summands for a sum etc.
    std::uint16_t nOperands(const Blob* header) noexcept;

    bool equal(const Blob* lhs, const Blob* rhs) noexcept;

    std::int16_t getSmallInt(Blob header) noexcept;
    SmallRational getSmallRational(Blob header) noexcept;
    double getFloatingPoint(const Blob* header) noexcept;
    LargeInt getLargeInt(const Blob* header);
    std::string_view getSymbolName(const Blob* header) noexcept;
    DomainFlag getDomainFlag(const Blob* header) noexcept;
    std::string_view getConstantName(const Blob* header) noexcept;
    std::string_view getFunctionName(const Blob* header) noexcept;
    UnaryDoubleFctPtr getUnaryDoubleFctPtr(const Blob* header) noexcept;
    BinaryDoubleFctPtr getBinaryDoubleFctPtr(const Blob* header) noexcept;

    // UB if the given header is not a complex number root node
    const Blob* getRealFromCommplexNumber(const Blob* header) noexcept;
    const Blob* getImagFromCommplexNumber(const Blob* header) noexcept;

    // UB if the given header is not a large rational root node
    const Blob* getNumeratorFromLargeRational(const Blob* header) noexcept;
    const Blob* getDenominatorFromLargeRational(const Blob* header) noexcept;

    // If there are no logical operands (e.g. for scalars), the first operand is identical to the
    // past-the-end/sentinel operand.
    const Blob* getFirstOperand(const Blob* e) noexcept;
    const Blob* getPastTheEndOperand(const Blob* e) noexcept;
}
