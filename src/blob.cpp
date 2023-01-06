
#include "blob.h"
#include <algorithm>
#include <bit>
#include <boost/iterator/function_output_iterator.hpp>
#include <cassert>
#include <cstring>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include "blob.h"

namespace sym2 {
    enum class Type : std::uint8_t {
        shortSymbol = 1, // Not starting at 0 helps pretty-printing in a debugger
        longSymbol,
        constant,
        smallInt,
        smallRational,
        floatingPoint,
        largeInt,
        // A large rational can still have one small/inplace integer (either numerator
        // or denominator):
        largeRational,
        complexNumber,
        sum,
        product,
        power,
        function
    };

    // There are two options for 8 byte data blobs to capture all desired leaf and composite types.
    // These constraints are subjet to the desired size - larger blob types could circumvent them
    // easily, but we stick to 8 bytes for the sake of a minimal memory footprint and optimal cache
    // usage.
    //
    // 1. Every blob instance can fully describe itself. This required a unique classifier for every
    // possible state of the union. Pro: simpler to understand, easier to pretty-print blobs in a
    // debugger, the type could have a meaningful API (member functions) because it always knows its
    // own state. Contra: a blob instance that represents a double must be fully preserved as is. An
    // implementation must hence hijack unused bits of a particular NaN state, and any query/member
    // function must do some bit masking magic to query the state or type. This is hard to implement
    // and might be expensive, as these queries will happen frequently and are the most basic
    // operations.
    //
    // 2. Externalise the contextual knowlege which kind of blob is usable in which way. A blob
    // instance can never fully know what itself represents. It might be a double, or the serialised
    // bytes of a large integer - in such a case, another instance is needed to inform about the
    // state/type. "Header" blobs will be responsible for providing the classification of blobs that
    // follow in a sequence of blobs. Pro: very compact representation, plain data that span several
    // blobs (long symbols or serialised large int bytes) can be easily used as no classification
    // byte needs to be ignored. Contra: We runtime-pessimize on accessing floating point data,
    // because it must be stored as a separate blob. An API doesn't make any sense, and we need to
    // deal with pretty opaque data, which is hard to reason about and hard to pretty-print in a
    // debugger. Bugs related to accidental type-punning will be harder to identify.
    //
    // We choose 2. for the sake of performance (without actually having measured it at this point,
    // oh well...) and an arguably easier implementation.
    union DataLayout {
        char largeSymbolData[8]; // More of a logical placeholder - will rarely be
                                 // used explicitly
        std::uint64_t largeIntData;
        double inexact;
        double (*unaryFctEval)(double);
        double (*binaryFctEval)(double, double);
        struct SelfDescribing {
            Type classifier;
            union DomainOrByte {
                DomainFlag domain;
                char byte;
            } pre0;
            char pre1;
            char pre2;
            union InplaceDataOrReference {
                char name[4];
                SmallRational exact;
                struct Referral {
                    std::uint16_t offset;
                    std::uint16_t extentOrOperands;
                } location;
            } main;
        } classified;

        static constexpr std::size_t smallSymbolNameLength = 6;
    };

    static_assert(std::is_trivial_v<Blob>);
    static_assert(std::is_trivial_v<DataLayout>);

    static_assert(sizeof(Blob) == sizeof(double));
    static_assert(alignof(Blob) == alignof(double));
    static_assert(alignof(Blob) == alignof(DataLayout));

    namespace {
        Blob toBlob(const DataLayout data)
        {
            return std::bit_cast<Blob>(data);
        }

        DataLayout fromBlob(const Blob data)
        {
            return std::bit_cast<DataLayout>(data);
        }

        const DataLayout* fromBlob(const Blob* data)
        {
            return reinterpret_cast<const DataLayout*>(data);
        }

        DataLayout* fromBlob(Blob* data)
        {
            return reinterpret_cast<DataLayout*>(data);
        }

        Type type(const Blob header) noexcept
        {
            return fromBlob(header).classified.classifier;
        }

        bool isSelfContainedHeader(const Blob header) noexcept
        {
            switch (type(header)) {
                case Type::shortSymbol:
                case Type::smallInt:
                case Type::smallRational:
                    return true;
                default:
                    return false;
            }
        }

        std::uint32_t extentFromBytes(const DataLayout data)
        {
            return data.classified.pre0.byte << 16 | data.classified.pre1 << 8
              | data.classified.pre2;
        }

        void setExtentAsBytes(const std::uint32_t extent, DataLayout& data)
        {
            if (extent > 1 << 24)
                throw std::range_error{"Extent too large to be stored, exceeds limit of 2^24"};

            data.classified.pre0.byte = static_cast<char>((extent >> 16) & 0xff);
            data.classified.pre1 = static_cast<char>((extent >> 8) & 0xff);
            data.classified.pre2 = static_cast<char>(extent & 0xff);
        }

        std::uint16_t offsetToRemote(const Blob header) noexcept
        {
            if (isSelfContainedHeader(header))
                return 0;
            else
                return fromBlob(header).classified.main.location.offset;
        }

        std::pair<std::uint16_t, std::uint32_t> offsetAndRemoteExtent(const Blob* const header)
        {
            return {offsetToRemote(*header), remoteExtent(header)};
        }
    }
}

bool sym2::isSmallName(const std::string_view name) noexcept
{
    return name.size() <= DataLayout::smallSymbolNameLength;
}

sym2::Blob sym2::construct(std::int16_t n) noexcept
{
    return toBlob(DataLayout{.classified = {.classifier = Type::smallInt,
                               .pre0 = {.byte = '\0'},
                               .pre1 = '\0',
                               .pre2 = '\0',
                               .main = {.exact = {n, 1}}}});
}

sym2::Blob sym2::construct(std::int16_t num, std::int16_t denom) noexcept
{
    assert(denom != 0);

    if (denom < 0) {
        num = static_cast<std::int16_t>(-num);
        denom = static_cast<std::int16_t>(-denom);
    }

    const auto divisor = std::gcd(num, denom);

    num = static_cast<std::int16_t>(num / divisor);
    denom = static_cast<std::int16_t>(denom / divisor);

    if (denom == std::int16_t{1})
        return construct(num);
    else
        return toBlob(DataLayout{.classified = {.classifier = Type::smallRational,
                                   .pre0 = {.byte = '\0'},
                                   .pre1 = '\0',
                                   .pre2 = '\0',
                                   .main = {.exact = {num, denom}}}});
}

sym2::Blob sym2::construct(const std::string_view symbolName, const DomainFlag domain) noexcept
{
    DataLayout result{.classified = {.classifier = Type::shortSymbol,
                        .pre0 = {.domain = domain},
                        .pre1 = '\0',
                        .pre2 = '\0',
                        .main = {.name = {'\0'}}}};

    std::memcpy(&result.classified.pre1, symbolName.data(), symbolName.size());

    return toBlob(result);
}

std::pmr::vector<sym2::Blob> sym2::constructSequence(
  double value, std::pmr::polymorphic_allocator<> alloc)
{
    return {{toBlob(DataLayout{.classified = {.classifier = Type::floatingPoint,
                                 .pre0 = {.byte = '\0'},
                                 .pre1 = '\0',
                                 .pre2 = '\0',
                                 .main = {.location = {1, 1}}}}),
              toBlob(DataLayout{.inexact = value})},
      alloc};
}

namespace sym2 {
    namespace {
        // When the name is short enough to fit into a standalone Blob, returns 0. Assumes that name
        // is no longer than 2^16, otherwise UB.
        std::uint16_t numRemoteBlobsForSymbolName(const std::string_view name)
        {
            if (isSmallName(name))
                return 0;

            const std::size_t nBytes = name.size();
            // Same as floating point ceil(nBytes/sizeof(Blob)):
            const std::size_t nBlobs = (nBytes + sizeof(Blob)) / sizeof(Blob);

            if (nBlobs + 1 > std::numeric_limits<std::uint16_t>::max())
                throw std::length_error{"Can only store names with 2^16 - 1 characters"};

            return static_cast<std::uint16_t>(nBlobs);
        }

        // The third parameter dictates the index at which the header blob of the symbol is placed
        // in the output container. If the output container is not large enough for this assignment,
        // it is resized. The name data is always appended at the end of the container, i.e.,
        // instead of overwriting existing blobs, new ones are appended, so the size of the output
        // container will always change.
        //
        // This function allocates exactly once. This is important to meet assumptions for
        // expressions backed by a fixed-size buffer.
        void appendLargeSymbol(std::string_view longName, DomainFlag domain,
          const std::size_t where, std::pmr::vector<Blob>& dest)
        {
            // In the general case, we operate on an output containter that might have existing
            // elements, but potentially also not enough elements to assign to dest[where]. Also,
            // there might be existing elements between dest[where] and dest.back(). Some examples:
            //
            // Input: dest = [], where = 0
            // Output: dest = [header, name...]
            //
            // Input: dest = [ blob, blob, blob, blob ], where = 5
            // Output: dest = [ blob, blob, blob, blob,  Blob{}, header, name... ]
            //
            // Input:  dest = [ blob, blob, blob, blob ], where = 2
            // Output: dest = [ blob, blob, header, blob, name... ]

            const std::size_t nBytes = longName.size();
            const std::uint16_t nBlobs = numRemoteBlobsForSymbolName(longName);

            dest.resize(std::max(dest.size(), where + 1) + nBlobs);

            const auto newOffset = static_cast<std::uint16_t>(dest.size() - nBlobs - where);

            dest[where] = toBlob(DataLayout{.classified = {.classifier = Type::longSymbol,
                                              .pre0 = {.domain = domain},
                                              .pre1 = '\0',
                                              .pre2 = '\0',
                                              .main = {.location = {newOffset, nBlobs}}}});

            char* nameBuffer = reinterpret_cast<char*>(dest.data() + where + newOffset);

            std::memcpy(nameBuffer, longName.data(), nBytes);

            // Append null byte since we have a long symbol anyhow.
            nameBuffer[nBytes] = '\0';
        }

        // Identical to appendLargeSymbol, but also covers the single-blob small symbol case
        void appendSmallOrLargeSymbol(std::string_view name, DomainFlag domain,
          const std::size_t where, std::pmr::vector<Blob>& dest)
        {
            if (name.length() <= DataLayout::smallSymbolNameLength)
                dest[where] = construct(name, domain);
            else
                appendLargeSymbol(name, domain, where, dest);
        }
    } // namespace
} // namespace sym2

std::pmr::vector<sym2::Blob> sym2::constructSequence(
  std::string_view symbolName, DomainFlag domain, std::pmr::polymorphic_allocator<> alloc)
{
    std::pmr::vector<Blob> result{alloc};

    appendLargeSymbol(symbolName, domain, 0, result);

    return result;
}

std::pmr::vector<sym2::Blob> sym2::constructSequence(
  std::string_view constantName, double value, std::pmr::polymorphic_allocator<> alloc)
{
    const std::uint16_t nBlobsForSymbol = 1 + numRemoteBlobsForSymbolName(constantName);
    // Double blob + symbol blobs:
    const std::uint32_t remoteExtent = 1 + nBlobsForSymbol;

    std::pmr::vector<Blob> result{1 + remoteExtent, alloc};

    result[0] = toBlob(DataLayout{.classified = {.classifier = Type::constant,
                                    .pre0 = {.byte = '\0'},
                                    .pre1 = '\0',
                                    .pre2 = '\0',
                                    .main = {.location = {1, 2}}}});
    result[1] = toBlob(DataLayout{.inexact = value});

    appendSmallOrLargeSymbol(constantName, DomainFlag::none, 2, result);

    setExtentAsBytes(remoteExtent, *fromBlob(&result[0]));

    return result;
}

namespace sym2 {
    namespace {
        std::uint16_t appendLargeIntData(const LargeInt& n, std::pmr::vector<Blob>& dest)
        {
            static_assert(sizeof(decltype(*n.backend().limbs())) == sizeof(Blob));

            const std::size_t currentSize = dest.size();
            const std::size_t dataSize = n.backend().size();

            if (dataSize > std::numeric_limits<std::uint16_t>::max()) {
                throw std::range_error{"Large integer limbs too large to store in a Blob sequence"};
            }

            dest.resize(currentSize + dataSize);

            export_bits(n, reinterpret_cast<std::uint64_t*>(&dest[currentSize]), sizeof(Blob) * 8);

            return static_cast<std::uint16_t>(dataSize);
        }

        void updateExtentInplace(Blob& header, const std::uint16_t extent)
        {
            DataLayout layout = fromBlob(header);
            layout.classified.main.location.extentOrOperands = extent;
            header = toBlob(layout);
        }

        void updateOffsetInplace(Blob& header, const std::uint16_t offset)
        {
            DataLayout layout = fromBlob(header);
            layout.classified.main.location.offset = offset;
            header = toBlob(layout);
        }
    } // namespace
} // namespace sym2

std::pmr::vector<sym2::Blob> sym2::constructSequence(
  const LargeInt& n, std::pmr::polymorphic_allocator<> alloc)
{
    std::pmr::vector<Blob> result{alloc};

    assert(!fitsInto<std::int16_t>(n));

    const std::size_t dataSize = n.backend().size();

    result.reserve(dataSize + 1);

    result.push_back(toBlob(DataLayout{.classified = {.classifier = Type::largeInt,
                                         // The limb data doesn't contain the sign bit, so store
                                         // this separately:
                                         .pre0 = {.byte = n < 0 ? char{1} : char{0}},
                                         .pre1 = '\0',
                                         .pre2 = '\0',
                                         .main = {.location = {1, 0 /* Set below */}}}}));

    const std::uint16_t extent = appendLargeIntData(n, result);

    updateExtentInplace(result.front(), extent);

    return result;
}

std::pmr::vector<sym2::Blob> sym2::constructSequence(
  const LargeRational& n, std::pmr::polymorphic_allocator<> alloc)
{
    const auto num = numerator(n);
    const auto denom = denominator(n);
    assert(!(fitsInto<std::int16_t>(num) && fitsInto<std::int16_t>(denom)));
    assert(denom > 0); // Satisfied by the LargeRational backend/implementation

    const std::size_t requiredSize = num.backend().size() + denom.backend().size();

    std::pmr::vector<Blob> result{alloc};
    // Make sure we only allocate once, no matter what callees reserve below.
    result.reserve(1 + requiredSize + 2);

    result.push_back(toBlob(DataLayout{.classified = {.classifier = Type::largeRational,
                                         .pre0 = {.byte = '\0'},
                                         .pre1 = '\0',
                                         .pre2 = '\0',
                                         .main = {.location = {1, 2}}}}));

    bool separateLargeNumData = false;
    bool separateLargeDenomData = false;

    if (fitsInto<std::int16_t>(num)) {
        result.push_back(construct(static_cast<std::int16_t>(num)));
    } else {
        result.push_back(toBlob(DataLayout{.classified = {.classifier = Type::largeInt,
                                             // The limb data doesn't contain the sign bit, so store
                                             // this separately:
                                             .pre0 = {.byte = n < 0 ? char{1} : char{0}},
                                             .pre1 = '\0',
                                             .pre2 = '\0',
                                             .main = {.location = {2, 0 /* Set below */}}}}));
        separateLargeNumData = true;
    }

    if (fitsInto<std::int16_t>(denom)) {
        result.push_back(construct(static_cast<std::int16_t>(denom)));
    } else {
        result.push_back(
          toBlob(DataLayout{.classified = {.classifier = Type::largeInt,
                              .pre0 = {.byte = '\0'},
                              .pre1 = '\0',
                              .pre2 = '\0',
                              .main = {.location = {0 /* Set below */, 0 /* Set below */}}}}));
        separateLargeDenomData = true;
    }

    std::uint16_t numDataSize = 0;

    if (separateLargeNumData) {
        numDataSize = appendLargeIntData(num, result);
        updateExtentInplace(result[1], numDataSize);
    }

    if (separateLargeDenomData) {
        const std::uint16_t denomDataSize = appendLargeIntData(denom, result);
        updateOffsetInplace(result[2], 1 + numDataSize);
        updateExtentInplace(result[2], denomDataSize);
    }

    return result;
}

std::pmr::vector<sym2::Blob> sym2::constructSequence(std::string_view function, const Blob* arg,
  UnaryDoubleFctPtr eval, std::pmr::polymorphic_allocator<> allocator)
{
    // Single-arg function blobs look like this:
    // 0: Root header
    // 1: Unary function pointer
    // 2: Small symbol, or large symbol header
    // 3: Function argument root/single blob
    // [4...]: Optional large symbol data
    // [...]: Optional function argument data

    const auto [argOffset, argExtent] = offsetAndRemoteExtent(arg);
    const std::uint32_t remoteExtent = 3 + argExtent + numRemoteBlobsForSymbolName(function);

    std::pmr::vector<Blob> result{allocator};
    // We account for root header, function pointer, symbol blob, plus the argument root blob.
    result.reserve(remoteExtent + 1);
    result.resize(4);

    result[0] = toBlob(DataLayout{.classified = {.classifier = Type::function,
                                    .pre0 = {.byte = '\0'},
                                    .pre1 = '\0',
                                    .pre2 = '\0',
                                    .main = {.location = {1, 1}}}});
    result[1] = toBlob(DataLayout{.unaryFctEval = eval});

    appendSmallOrLargeSymbol(function, DomainFlag::none, 2, result);
    appendDuplicateSequence(arg, 3, result);

    setExtentAsBytes(remoteExtent, *fromBlob(&result[0]));

    return result;
}

std::pmr::vector<sym2::Blob> sym2::constructSequence(std::string_view function, const Blob* arg1,
  const Blob* arg2, BinaryDoubleFctPtr eval, std::pmr::polymorphic_allocator<> allocator)
{
    std::pmr::vector<Blob> result{allocator};
    const auto [offset1, extent1] = offsetAndRemoteExtent(arg1);
    const auto [offset2, extent2] = offsetAndRemoteExtent(arg2);
    // Function header, function pointer, symbol root blob, both argument root blobs, and optionally
    // remote extent of large symbol and the arguments.
    const std::uint32_t remoteExtent =
      4 + extent1 + extent2 + numRemoteBlobsForSymbolName(function);

    result.reserve(remoteExtent + 1);
    result.resize(5);

    // Binary function blobs look like unary ones, except that they have two arguments.
    result[0] = toBlob(DataLayout{.classified = {.classifier = Type::function,
                                    .pre0 = {.byte = '\0'},
                                    .pre1 = '\0',
                                    .pre2 = '\0',
                                    .main = {.location = {1, 2}}}});
    result[1] = toBlob(DataLayout{.binaryFctEval = eval});

    appendSmallOrLargeSymbol(function, DomainFlag::none, 2, result);
    appendDuplicateSequence(arg1, 3, result);
    appendDuplicateSequence(arg2, 4, result);

    setExtentAsBytes(remoteExtent, *fromBlob(&result[0]));

    return result;
}

namespace sym2 {
    namespace {
        Type toInternalType(const CompositeType composite)
        {
            switch (composite) {
                case CompositeType::sum:
                    return Type::sum;
                case CompositeType::product:
                    return Type::product;
                case CompositeType::power:
                    return Type::power;
                case CompositeType::complexNumber:
                    return Type::complexNumber;
                default:
                    assert(false && "Unhandled composite type");
                    return Type::sum; // Random choice
            }
        }
    }
}

sym2::Blob sym2::constructCompositeHeader(
  CompositeType composite, const std::uint16_t numOperands, const std::uint32_t extent) noexcept
{
    DataLayout data{.classified = {.classifier = toInternalType(composite),
                      .pre0 = {.byte = '\0'},
                      .pre1 = '\0',
                      .pre2 = '\0',
                      .main = {.location = {1, numOperands}}}};

    setExtentAsBytes(extent, data);

    return toBlob(data);
}

namespace sym2 {
    namespace {
        // Duplicates a blob and sets the offset to the given new offset. Works only with a header
        // blob that is followed by additional data, UB otherwise.
        Blob constructDuplicate(Blob header, std::uint16_t newOffset) noexcept
        {
            DataLayout result = fromBlob(header);

            result.classified.main.location.offset = newOffset;

            return toBlob(result);
        }
    }
}

std::pmr::vector<sym2::Blob> sym2::constructDuplicateSequence(
  const Blob* from, std::pmr::polymorphic_allocator<> allocator)
{
    std::pmr::vector<Blob> result{allocator};

    appendDuplicateSequence(from, 0, result);

    return result;
}

void sym2::appendDuplicateSequence(
  const Blob* from, std::size_t where, std::pmr::vector<Blob>& output)
{
    if (output.size() < where + 1)
        output.resize(where + 1);

    if (isSelfContainedHeader(*from)) {
        output[where] = *from;
    } else {
        output[where] =
          constructDuplicate(*from, static_cast<std::uint16_t>(output.size() - where));

        const auto [offset, extent] = offsetAndRemoteExtent(from);
        const std::size_t currentSize = output.size();

        output.resize(currentSize + extent);

        std::copy(from + offset, from + offset + extent, output.begin() + currentSize);
    }
}

bool sym2::isNumberHeader(const Blob header) noexcept
{
    switch (type(header)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::floatingPoint:
        case Type::largeInt:
        case Type::largeRational:
        case Type::complexNumber:
            return true;
        default:
            return false;
    }
}

bool sym2::isIntegerHeader(const Blob header) noexcept
{
    const Type t = type(header);

    return t == Type::smallInt || t == Type::largeInt;
}

bool sym2::isRationalHeader(const Blob header) noexcept
{
    switch (type(header)) {
        case Type::smallInt:
        case Type::smallRational:
        case Type::largeInt:
        case Type::largeRational:
            return true;
        default:
            return false;
    }
}

bool sym2::isFloatingPointHeader(const Blob header) noexcept
{
    return type(header) == Type::floatingPoint;
}

bool sym2::isComplexNumberHeader(const Blob header) noexcept
{
    return type(header) == Type::complexNumber;
}

bool sym2::isSmallHeader(const Blob header) noexcept
{
    switch (type(header)) {
        case Type::smallInt:
        case Type::smallRational:
            return true;
        default:
            return false;
    }
}

bool sym2::isLargeHeader(const Blob header) noexcept
{
    return !isSmallHeader(header);
}

bool sym2::isScalarHeader(const Blob header) noexcept
{
    return !isCompositeHeader(header);
}

bool sym2::isCompositeHeader(const Blob header) noexcept
{
    switch (type(header)) {
        case Type::sum:
        case Type::product:
        case Type::power:
        case Type::function:
            return true;
        default:
            return false;
    }
}

bool sym2::isSymbolHeader(const Blob header) noexcept
{
    const Type t = type(header);
    return t == Type::shortSymbol || t == Type::longSymbol;
}

bool sym2::isConstantHeader(const Blob header) noexcept
{
    return type(header) == Type::constant;
}

bool sym2::isSumHeader(const Blob header) noexcept
{
    return type(header) == Type::sum;
}

bool sym2::isProductHeader(const Blob header) noexcept
{
    return type(header) == Type::product;
}

bool sym2::isPowerHeader(const Blob header) noexcept
{
    return type(header) == Type::power;
}

bool sym2::isFunctionHeader(const Blob header) noexcept
{
    return type(header) == Type::function;
}

std::uint32_t sym2::remoteExtent(const Blob* const header) noexcept
{
    switch (type(*header)) {
        case Type::shortSymbol:
        case Type::smallInt:
        case Type::smallRational:
            return 0;
        case Type::floatingPoint:
            return 1;
        case Type::longSymbol:
        case Type::largeInt:
            return fromBlob(*header).classified.main.location.extentOrOperands;
        case Type::largeRational:
            return remoteExtent(header + offsetToRemote(*header))
              + remoteExtent(header + offsetToRemote(*header) + 1);
        case Type::constant:
        case Type::complexNumber:
        case Type::sum:
        case Type::product:
        case Type::power:
        case Type::function:
            return extentFromBytes(fromBlob(*header));
        default:
            assert(false);
            return 0;
    }
}

std::uint16_t sym2::nOperands(const Blob* const header) noexcept
{
    switch (type(*header)) {
        case Type::shortSymbol:
        case Type::smallInt:
        case Type::smallRational:
        case Type::longSymbol:
        case Type::largeInt:
        case Type::floatingPoint:
        case Type::constant:
        case Type::largeRational:
        case Type::complexNumber:
            return 0;
        case Type::power:
            return 2;
        case Type::sum:
        case Type::product:
        case Type::function:
            return fromBlob(*header).classified.main.location.extentOrOperands;
        default:
            assert(false);
            return 0;
    }
}

bool sym2::equal(const Blob* const lhs, const Blob* const rhs) noexcept
{
    // Optimisation idea for this function: bitcast both blobs into a 64bit integer, apply a bit
    // mask that zeros out the offset (which must not be compared when there's remote blobs), and
    // compare the integers. However, for a standalone blob like a small rational number, we still
    // need to compare these 16bits, so the mask must be conditional. One possible way to retrieve
    // this conditional mask without branching could be perform a std::bitset lookup using the type
    // enumeration as an index. The resulting bit could be used to construct this conditional mask.
    if (type(*lhs) != type(*rhs))
        return false;

    if (isSelfContainedHeader(*lhs)) {
        assert(isSelfContainedHeader(*rhs));

        return std::memcmp(lhs, rhs, sizeof(Blob)) == 0;
    }

    const auto [lhsOffset, lhsExtent] = offsetAndRemoteExtent(lhs);
    const auto [rhsOffset, rhsExtent] = offsetAndRemoteExtent(rhs);
    const std::uint16_t lhsNumOperands = nOperands(lhs);
    const std::uint16_t rhsNumOperands = nOperands(rhs);

    assert(lhsExtent > 0 && rhsExtent > 0);

    if (lhsExtent != rhsExtent || lhsNumOperands != rhsNumOperands)
        return false;

    return std::memcmp(lhs + lhsOffset, rhs + rhsOffset, lhsExtent * sizeof(Blob)) == 0;
}

std::int16_t sym2::getSmallInt(Blob header) noexcept
{
    assert(isIntegerHeader(header) && isSmallHeader(header));

    return fromBlob(header).classified.main.exact.num;
}

sym2::SmallRational sym2::getSmallRational(Blob header) noexcept
{
    assert(isRationalHeader(header) && isSmallHeader(header));

    return fromBlob(header).classified.main.exact;
}

double sym2::getFloatingPoint(const Blob* const header) noexcept
{
    assert(isFloatingPointHeader(*header) || isConstantHeader(*header));

    const std::uint32_t offset = offsetToRemote(*header);

    return fromBlob(*std::next(header, offset)).inexact;
}

sym2::LargeInt sym2::getLargeInt(const Blob* const header)
{
    assert(isIntegerHeader(*header) && isLargeHeader(*header));

    LargeInt result;

    const auto [offset, extent] = offsetAndRemoteExtent(header);

    const std::span<const Blob> limbBytes{std::next(header, offset), extent};
    const auto* first = reinterpret_cast<const std::uint64_t*>(limbBytes.data());
    const auto* last = std::next(first, limbBytes.size());

    import_bits(result, first, last);

    const short sign = fromBlob(*header).classified.pre0.byte ? -1 : 1;

    return result * sign;
}

std::string_view sym2::getSymbolName(const Blob* const header) noexcept
{
    assert(isSymbolHeader(*header));

    if (type(*header) == Type::shortSymbol) {
        // Important to not use a new object, but alias the given argument in order to return a
        // valid view.
        const std::string_view name{
          &fromBlob(header)->classified.pre1, DataLayout::smallSymbolNameLength};
        const std::size_t null = name.find_first_of('\0');

        return null == std::string_view::npos ? name : name.substr(0, null);
    } else {
        const std::uint16_t offset = offsetToRemote(*header);
        return std::string_view{reinterpret_cast<const char*>(std::next(header, offset))};
    }
}

sym2::DomainFlag sym2::getDomainFlag(const Blob* header) noexcept
{
    return fromBlob(*header).classified.pre0.domain;
}

std::string_view sym2::getConstantName(const Blob* header) noexcept
{
    assert(isConstantHeader(*header));

    const std::uint16_t offset = offsetToRemote(*header);

    return getSymbolName(std::next(header, offset + 1));
}

std::string_view sym2::getFunctionName(const Blob* header) noexcept
{
    assert(isFunctionHeader(*header));
    const std::uint16_t offset = offsetToRemote(*header);

    return getSymbolName(header + offset + 1);
}

sym2::UnaryDoubleFctPtr sym2::getUnaryDoubleFctPtr(const Blob* const header) noexcept
{
    assert(isFunctionHeader(*header));
    const std::uint16_t offset = offsetToRemote(*header);

    return fromBlob(*(header + offset)).unaryFctEval;
}

sym2::BinaryDoubleFctPtr sym2::getBinaryDoubleFctPtr(const Blob* const header) noexcept
{
    assert(isFunctionHeader(*header));
    const std::uint16_t offset = offsetToRemote(*header);

    return fromBlob(*(header + offset)).binaryFctEval;
}

const sym2::Blob* sym2::getRealFromCommplexNumber(const Blob* header) noexcept
{
    assert(isComplexNumberHeader(*header));

    return header + offsetToRemote(*header);
}

const sym2::Blob* sym2::getImagFromCommplexNumber(const Blob* header) noexcept
{
    return getRealFromCommplexNumber(header) + 1;
}

const sym2::Blob* sym2::getNumeratorFromLargeRational(const Blob* header) noexcept
{
    assert(isLargeHeader(*header) && isRationalHeader(*header));

    return header + offsetToRemote(*header);
}

const sym2::Blob* sym2::getDenominatorFromLargeRational(const Blob* header) noexcept
{
    return getNumeratorFromLargeRational(header) + 1;
}

const sym2::Blob* sym2::getFirstOperand(const Blob* e) noexcept
{
    // See function to create functions, there are physical operands that we don't treat as logical
    // ones.
    const std::uint16_t delta = isFunctionHeader(*e) ? 2 : 0;

    return e + offsetToRemote(*e) + delta;
}

const sym2::Blob* sym2::getPastTheEndOperand(const Blob* e) noexcept
{
    const std::uint16_t delta = isFunctionHeader(*e) ? 2 : 0;

    return e + offsetToRemote(*e) + nOperands(e) + delta;
}
