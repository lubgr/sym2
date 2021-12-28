#pragma once

#include <array>
#include <cstddef>
#include <memory_resource>

namespace sym2 {
    namespace detail {
        std::pmr::memory_resource* currentOrDefaultResource();
    }

    /* A somewhat hacky and even unsafe (no virtual or protected base class destructor) workaround for using the pmr
     * allocator together with cpp_int from Boost multiprecision. The issue this fixes is that cpp_int_backend requires
     * the allocator to be copy assignable, which std::pmr::polymorphic_allocator doesn't satisfy. */
    template <class T>
    class LargeIntAllocator : public std::pmr::polymorphic_allocator<T> {
      public:
        LargeIntAllocator()
            : std::pmr::polymorphic_allocator<T>{detail::currentOrDefaultResource()}
        {}

        LargeIntAllocator(const LargeIntAllocator&) = default;
        LargeIntAllocator& operator=(const LargeIntAllocator&)
        {
            static_assert(!std::allocator_traits<
                          std::pmr::polymorphic_allocator<int>>::propagate_on_container_move_assignment::value);

            // Just keep the current allocator. Most of the time, the allocators of this and the other instance should
            // be indentical anyhow (even more, most of the time we shouldn't copy-assign large integers).
            return *this;
        }
    };

    class LargeIntResourceRegistrar {
      public:
        explicit LargeIntResourceRegistrar(std::pmr::memory_resource* resource);
        ~LargeIntResourceRegistrar();

      private:
        std::pmr::memory_resource* const resource;
    };

    enum class ByteSize : std::size_t {};
    template <ByteSize N>
    auto monotonicStackPmrResource()
    {
        constexpr std::size_t bufferSize = static_cast<std::underlying_type_t<ByteSize>>(N);
        struct BufferAndResourceRegistrar {
            std::array<std::byte, bufferSize> buffer;
            LargeIntResourceRegistrar registrar;
        };
        struct RegisteredBufferAndAllocator {
            BufferAndResourceRegistrar bufferAndRegistrar{{}, LargeIntResourceRegistrar{&this->resource}};
            std::pmr::monotonic_buffer_resource resource{
              bufferAndRegistrar.buffer.data(), bufferAndRegistrar.buffer.size()};
        };

        return RegisteredBufferAndAllocator{};
    }
}
