#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <new>
#include <scoped_allocator>
#include <vector>

namespace sym2 {
    // These allocator facilities build upon Howard Hinnant's stack allocator, which is published
    // under the MIT license. See LICENSE for a copy of the license, and
    // http://stackoverflow.com/q/33722907/576911 or
    // https://howardhinnant.github.io/stack_alloc.html for more context.
    class BufferBase {
      public:
        constexpr BufferBase(char* const buffer, const std::size_t n, const std::size_t align,
          BufferBase* upstream, bool operator_new_delete_fallback = true) noexcept
            : buffer{buffer}
            , current{buffer}
            , n{n}
            , align{align}
            , upstream{upstream}
            , global_fallback{operator_new_delete_fallback}
        {}
        BufferBase(const BufferBase&) = delete;
        BufferBase& operator=(const BufferBase&) = delete;

        template <std::size_t alignmentRequest>
        constexpr char* allocate(const std::size_t bytes)
        {
            assert(alignmentRequest <= align);
            assert(IsInBuffer(current));

            auto const alignedBytes = alignUp(bytes);

            if (static_cast<std::size_t>(buffer + n - current) >= alignedBytes) {
                char* const result = current;
                current += alignedBytes;
                return result;
            } else if (upstream) {
                return upstream->allocate<alignmentRequest>(bytes);
            } else if (global_fallback) {
                return reinterpret_cast<char*>(
                  ::operator new(bytes, std::align_val_t{alignmentRequest}));
            }

            throw std::bad_alloc{};
        }

        template <std::size_t alignmentRequest>
        constexpr void deallocate(char* const ptr, const std::size_t bytes) noexcept
        {
            assert(alignmentRequest <= align);
            assert(IsInBuffer(current));

            if (IsInBuffer(ptr)) {
                if (ptr + alignUp(bytes) == current)
                    current = ptr;

                return;
            } else if (upstream) {
                return upstream->deallocate<alignmentRequest>(ptr, bytes);
            } else if (global_fallback) {
                return ::operator delete(ptr, std::align_val_t{alignmentRequest});
            }

            assert(false);
        }

      protected:
        ~BufferBase()
        {
            current = nullptr;
        }

      private:
        constexpr std::size_t alignUp(std::size_t bytes) noexcept
        {
            return (bytes + (align - 1)) & ~(align - 1);
        }

        bool IsInBuffer(char* ptr) noexcept
        {
            return std::uintptr_t(buffer) <= std::uintptr_t(ptr)
              && std::uintptr_t(ptr) <= std::uintptr_t(buffer) + n;
        }

        char* buffer;
        char* current;
        std::size_t n;
        std::size_t align;
        BufferBase* upstream;
        bool global_fallback;
    };

    template <std::size_t bufferSize, std::size_t bufferAlignment = alignof(std::max_align_t)>
    class StackBuffer : public BufferBase {
      public:
        explicit StackBuffer(
          BufferBase* upstream = nullptr, bool operator_new_delete_fallback = true) noexcept
            : BufferBase{
              buffer, bufferSize, bufferAlignment, upstream, operator_new_delete_fallback}
        {
            static_assert(bufferSize % bufferAlignment == 0);
        }
        StackBuffer(const StackBuffer&) = delete;
        StackBuffer& operator=(const StackBuffer&) = delete;
        ~StackBuffer() = default;

      private:
        alignas(bufferAlignment) char buffer[bufferSize];
    };

    template <class T = std::byte>
    class LocalAlloc {
      public:
        using value_type = T;

        template <class>
        friend class LocalAlloc;

        LocalAlloc(BufferBase* const buffer = nullptr)
            : buffer{buffer}
        {}
        template <class U>
        LocalAlloc(const LocalAlloc<U>& other) noexcept
            : buffer(other.buffer)
        {}
        LocalAlloc(const LocalAlloc&) = default;
        LocalAlloc& operator=(const LocalAlloc&) = delete;
        ~LocalAlloc() = default;

        template <class U>
        struct rebind {
            using other = LocalAlloc<U>;
        };

        T* allocate(const std::size_t n)
        {
            constexpr std::size_t align = alignof(T);
            const std::size_t bytes = n * sizeof(T);

            if (buffer)
                return reinterpret_cast<T*>(buffer->template allocate<align>(bytes));
            else
                return reinterpret_cast<T*>(::operator new(bytes, std::align_val_t{align}));
        }

        void deallocate(T* ptr, std::size_t n) noexcept
        {
            constexpr std::size_t align = alignof(T);
            const std::size_t bytes = n * sizeof(T);

            if (buffer)
                buffer->template deallocate<align>(reinterpret_cast<char*>(ptr), bytes);
            else
                ::operator delete(ptr, std::align_val_t{align});
        }

        BufferBase* getBuffer()
        {
            return buffer;
        }

        template <class U>
        bool operator==(const LocalAlloc<U>& other) noexcept
        {
            return buffer == other.buffer;
        }

        template <class U>
        bool operator!=(const LocalAlloc<U>& other) noexcept
        {
            return !this->operator==(other);
        }

      private:
        BufferBase* buffer;
    };

    template <class T>
    using LocalVec = std::vector<T, LocalAlloc<T>>;
    template <class T>
    using ScopedLocalAlloc = std::scoped_allocator_adaptor<LocalAlloc<T>>;
    template <class T>
    using ScopedLocalVec = std::vector<T, ScopedLocalAlloc<T>>;
}
