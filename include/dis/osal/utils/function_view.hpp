#ifndef DIS_OSAL_UTILS_FUNCTION_VIEW_HPP
#define DIS_OSAL_UTILS_FUNCTION_VIEW_HPP

// NOTE: function view implementation base on:
// https://www.foonathan.net/2017/01/function-ref-implementation/

#include "dis/osal/debug/contracts.hpp"

#include <type_traits>
#include <algorithm>

namespace dis::detail {
template <typename... Types>
struct aligned_union {
    static constexpr auto size_value      = std::max(sizeof(Types)...);
    static constexpr auto alignment_value = std::max(alignof(Types)...);

    using type =
        typename std::aligned_storage<size_value, alignment_value>::type;
};

template <typename... Types>
using aligned_union_t = typename aligned_union<Types...>::type;
}  // namespace dis::detail

namespace dis {

template <typename Signature>
class function_view;

template <typename RETURN_T, typename... ARG_Ts>
class function_view<RETURN_T(ARG_Ts...)> {
public:
    using signature = RETURN_T(ARG_Ts...);

    function_view(RETURN_T (*fptr)(ARG_Ts...)) {
        using pointer_type = RETURN_T (*)(ARG_Ts...);
        dis_expects(fptr != nullptr);

        ::new (get_memory()) pointer_type(fptr);

        m_callback = [](const void* memory, ARG_Ts... args) {
            auto func = *static_cast<const pointer_type*>(memory);
            return func(static_cast<ARG_Ts>(args)...);
        };
    }

    template <typename FUNCTOR_T>
    explicit function_view(FUNCTOR_T& functor)
        : m_callback(&invoke_functor<FUNCTOR_T>) {
        ::new (get_memory()) void*(&functor);
    }

    RETURN_T operator()(ARG_Ts... args) const {
        return m_callback(get_memory(), static_cast<ARG_Ts>(args)...);
    }

private:
    using storage =
        dis::detail::aligned_union_t<void*, RETURN_T (*)(ARG_Ts...)>;
    using callback = RETURN_T (*)(const void*, ARG_Ts...);

    void* get_memory() noexcept { return &m_storage; }
    const void* get_memory() const noexcept { return &m_storage; }

    template <typename FUNCTOR_T>
    static RETURN_T invoke_functor(const void* memory, ARG_Ts... args) {
        using ptr_t = void*;
        auto ptr    = *static_cast<const ptr_t*>(memory);
        auto& func  = *static_cast<FUNCTOR_T*>(ptr);
        // deliberately assumes operator(), see further below
        return static_cast<RETURN_T>(func(static_cast<ARG_Ts>(args)...));
    }

    storage m_storage;
    callback m_callback;
};

}  // namespace dis

#endif  // DIS_OSAL_UTILS_FUNCTION_VIEW_HPP
