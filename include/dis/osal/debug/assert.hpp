#ifndef DIS_OSAL_DEBUG_ASSERT_HPP
#define DIS_OSAL_DEBUG_ASSERT_HPP

// NOTE: assertion implementation based on:
// https://www.foonathan.net/2016/09/assertions/

#include <cstdio>   // for fprintf
#include <cstdlib>  // for abort
#include <source_location>
#include <type_traits>
#include <utility>

namespace dis {
template <unsigned LEVEL_V>
using assert_level = ::std::integral_constant<unsigned, LEVEL_V>;

struct no_assert_handler {
    template <typename... ARG_Ts>
    static void handle(const ::std::source_location&,
                       const char*,
                       ARG_Ts&&...) noexcept {}

    static constexpr unsigned level_value = 0;
};

struct printf_abort_handler {
    static void handle(const ::std::source_location& loc,
                       const char* expression) noexcept {
        std::fprintf(stderr, "[assert] %s:%s[%lu]: '%s' failed.\n",
                     loc.function_name(), loc.file_name(), loc.line(),
                     expression);
        ::std::abort();
    }

    static void handle(const ::std::source_location& loc,
                       const char* expression,
                       const char* message) noexcept {
        std::fprintf(stderr, "[assert] %s:%s[%lu]: '%s' failed - %s.\n",
                     loc.function_name(), loc.file_name(), loc.line(),
                     expression, message);
        ::std::abort();
    }

    static constexpr unsigned level_value = 3;
};

}  // namespace dis

namespace dis::detail {
// overload 1, with level, enabled
template <class EXPR_T, class HANDLER_T, unsigned LEVEL_V, typename... ARG_Ts>
    requires(LEVEL_V <= HANDLER_T::level_value)
void do_assert(const EXPR_T& expr,
               const ::std::source_location& loc,
               const char* expression,
               HANDLER_T,
               ::dis::assert_level<LEVEL_V>,
               ARG_Ts&&... args) noexcept {
    static_assert(LEVEL_V > 0, "level of an assertion must not be 0");
    if (!expr()) {
        HANDLER_T::handle(loc, expression, std::forward<ARG_Ts>(args)...);
    }
}

// overload 1, with level, disabled
template <class EXPR_T, class HANDLER_T, unsigned LEVEL_V, typename... ARG_Ts>
    requires(LEVEL_V > HANDLER_T::level_value)
void do_assert(const EXPR_T&,
               const ::std::source_location&,
               const char*,
               HANDLER_T,
               ::dis::assert_level<LEVEL_V>,
               ARG_Ts&&...) noexcept {}

// overload 2, without level, disabled
template <class EXPR_T, class HANDLER_T, typename... ARG_Ts>
    requires(HANDLER_T::level_value == 0)
void do_assert(const EXPR_T&,
               const ::std::source_location&,
               const char*,
               HANDLER_T,
               ARG_Ts&&...) noexcept {}

// overload 2, without level, enabled
template <class EXPR_T, class HANDLER_T, typename... ARG_Ts>
    requires(HANDLER_T::level_value != 0)
void do_assert(const EXPR_T& expr,
               const ::std::source_location& loc,
               const char* expression,
               HANDLER_T,
               ARG_Ts&&... args) noexcept {
    if (!expr()) {
        HANDLER_T::handle(loc, expression, std::forward<ARG_Ts>(args)...);
    }
}

}  // namespace dis::detail

#define dis_assert(Expr, ...)                                        \
    dis::detail::do_assert([&] { return Expr; },                     \
                           ::std::source_location::current(), #Expr, \
                           __VA_ARGS__)

#endif  // DIS_OSAL_DEBUG_ASSERT_HPP
