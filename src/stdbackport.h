#pragma once

#include <concepts>
#include <type_traits>

namespace stdbackport {
    template <class _From, class _To>
    concept convertible_to = std::is_convertible_v<_From, _To> && requires(std::add_rvalue_reference_t<_From> (&__f)())
    {
        static_cast<_To>(__f());
    };

    template <class _Tp>
    concept __boolean_testable_impl = convertible_to<_Tp, bool>;

    template <class _Tp>
    concept __boolean_testable = __boolean_testable_impl<_Tp> && requires(_Tp&& __t)
    {
        {
            !std::forward<_Tp>(__t)
            } -> __boolean_testable_impl;
    };

    template <class _Fn, class... _Args>
    concept invocable = requires(_Fn&& __fn, _Args&&... __args)
    {
        std::invoke(std::forward<_Fn>(__fn), std::forward<_Args>(__args)...); // not required to be equality preserving
    };

    template <class _Fn, class... _Args>
    concept regular_invocable = invocable<_Fn, _Args...>;

    template <class _Fn, class... _Args>
    concept predicate = regular_invocable<_Fn, _Args...> && __boolean_testable<std::invoke_result_t<_Fn, _Args...>>;
}
