#ifndef DIS_OSAL_THREAD_DETAIL_SEMAPHORE_POLICY_HPP
#define DIS_OSAL_THREAD_DETAIL_SEMAPHORE_POLICY_HPP

#include "dis/osal/utils/cpu.hpp"
#include "dis/osal/debug/contracts.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

namespace dis::detail {

struct mutex_tag {};
struct cnt_semaphore_tag {};
struct bin_semaphore_tag {};

struct semaphore_policy {
    using count_type         = std::ptrdiff_t;
    using native_handle_type = ::SemaphoreHandle_t;

    static inline void destroy(native_handle_type& handle) noexcept {
        if (handle) {
            ::vSemaphoreDelete(handle);
        }
    }

    static inline auto create(mutex_tag) noexcept -> native_handle_type {
        dis_expects((!this_cpu::is_in_isr()));
        native_handle_type mtx = ::xSemaphoreCreateMutex();
        dis_ensures((mtx != nullptr));
        return mtx;
    }

    template <count_type LEAST_MAX_VALUE>
    static inline auto create(count_type desired) noexcept
        -> native_handle_type {
        if constexpr (LEAST_MAX_VALUE == 1) {
            return create(bin_semaphore_tag{}, desired);
        } else {
            return create(cnt_semaphore_tag{}, LEAST_MAX_VALUE, desired);
        }
    }

    static inline auto create(cnt_semaphore_tag,
                              count_type max,
                              count_type desired) noexcept
        -> native_handle_type {
        dis_expects((!this_cpu::is_in_isr()));
        dis_expects((desired <= max));
        native_handle_type sem = ::xSemaphoreCreateCounting(max, desired);
        dis_ensures((sem != nullptr));
        return sem;
    }

    static inline auto create(bin_semaphore_tag, count_type desired) noexcept
        -> native_handle_type {
        dis_expects((!this_cpu::is_in_isr()));
        dis_expects((desired <= 1));
        native_handle_type sem = ::xSemaphoreCreateBinary();
        dis_ensures((sem != nullptr));
        take(sem, desired);
        return sem;
    }

    static inline bool take(native_handle_type& sem,
                            TickType_t ticks) noexcept {
        if (!this_cpu::is_in_isr()) {
            return (::xSemaphoreTake(sem, ticks) == pdTRUE);
        }

        ::BaseType_t needs_yield = pdFALSE;
        const bool success =
            (::xSemaphoreTakeFromISR(sem, &needs_yield) == pdTRUE);
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    static inline bool give(native_handle_type& sem,
                            count_type desired = 1) noexcept {
        if (!this_cpu::is_in_isr()) {
            while ((desired > 0)) {
                if (::xSemaphoreGive(sem) != pdPASS) {
                    return false;
                }
                desired--;
            }
            return true;
        }

        ::BaseType_t needs_yield = pdFALSE;
        bool success             = true;
        while (success && (desired > 0)) {
            success = (::xSemaphoreGiveFromISR(sem, &needs_yield) == pdTRUE);
            desired--;
        }
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }
};

}  // namespace dis::detail

#endif  // DIS_OSAL_THREAD_DETAIL_SEMAPHORE_POLICY_HPP
