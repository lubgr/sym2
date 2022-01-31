/* Adjusted from LLVM, llvm/ADT/STLExtras.h, see the LICENSE file of sym2 for more details and a
 * copy of the license.
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions. See
 * https://llvm.org/LICENSE.txt for license information. SPDX-License-Identifier: Apache-2.0 WITH
 * LLVM-exception */

#pragma once
#include <cstdint>
#include <type_traits>

namespace sym2 {
    template <typename Fct>
    class FunctionView;

    template <typename ReturnType, typename... T>
    class FunctionView<ReturnType(T...)> {
      public:
        template <typename Fct>
        FunctionView(Fct&& callable,
          std::enable_if_t<!std::is_same_v<std::remove_cv_t<std::remove_reference_t<Fct>>,
            FunctionView>>* = nullptr)
            : callback(invoke<std::remove_reference_t<Fct>>)
            , callable(reinterpret_cast<void*>(&callable))
        {}

        ReturnType operator()(T... args) const
        {
            return callback(callable, std::forward<T>(args)...);
        }

      private:
        template <typename Fct>
        static ReturnType invoke(void* callable, T... args)
        {
            return (*reinterpret_cast<Fct*>(callable))(std::forward<T>(args)...);
        }

        ReturnType (*callback)(void* callable, T... args);
        void* callable;
    };
}
