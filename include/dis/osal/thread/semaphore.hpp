#ifndef DIS_OSAL_THREAD_SEMAPHORE_HPP
#define DIS_OSAL_THREAD_SEMAPHORE_HPP

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/utils/cpu.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

namespace dis {

template <std::ptrdiff_t LEAST_MAX_V>
class counting_semaphore {
public:
    using count_type         = std::ptrdiff_t;
    using native_handle_type = ::SemaphoreHandle_t;
    counting_semaphore(count_type desired = 0) noexcept
        : m_handle{::xSemaphoreCreateCounting(LEAST_MAX_V, desired)} {}
    ~counting_semaphore() noexcept {
        if (m_handle) {
            ::vSemaphoreDelete(m_handle);
        }
    }

    counting_semaphore(counting_semaphore&& other) noexcept
        : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    counting_semaphore& operator=(counting_semaphore&& other) noexcept {
        counting_semaphore(std::move(other)).swap(*this);
        return *this;
    }

    counting_semaphore(const counting_semaphore&)            = delete;
    counting_semaphore& operator=(const counting_semaphore&) = delete;

    inline void swap(counting_semaphore& other) noexcept {
        using std::swap;
        swap(m_handle, other.m_handle);
    }

    inline void release(count_type update = 1) noexcept { give(update); }
    inline void aquire() { take(freertos::infinity_delay); }
    [[nodiscard]] inline bool try_aquire() { return take(0); }

    template <class REP_T, class PERIOD_T>
    [[nodiscard]] inline bool try_aquire_for(
        const std::chrono::duration<REP_T, PERIOD_T>& time) noexcept {
        return take(freertos::to_ticks(time));
    }

private:
    inline bool take(TickType_t ticks) noexcept {
        if (!this_cpu::is_in_isr()) {
            return (::xSemaphoreTake(m_handle, ticks) == pdTRUE);
        }

        ::BaseType_t needs_yield = pdFALSE;
        const bool success =
            (::xSemaphoreTakeFromISR(m_handle, &needs_yield) == pdTRUE);
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    inline bool give(count_type update) noexcept {
        if (!this_cpu::is_in_isr()) {
            while ((update > 0)) {
                if (::xSemaphoreGive(m_handle) != pdPASS) {
                    return false;
                }
                update--;
            }
            return true;
        }

        ::BaseType_t needs_yield = pdFALSE;
        bool success             = true;
        while (success && (update > 0)) {
            success =
                (::xSemaphoreGiveFromISR(m_handle, &needs_yield) == pdTRUE);
            update--;
        }
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    native_handle_type m_handle{nullptr};
};

template <>
class counting_semaphore<1> {
public:
    using count_type         = std::ptrdiff_t;
    using native_handle_type = ::SemaphoreHandle_t;
    counting_semaphore(count_type desired = 0) noexcept
        : m_handle{::xSemaphoreCreateBinary()} {
        give(desired);
    }
    ~counting_semaphore() noexcept {
        if (m_handle) {
            ::vSemaphoreDelete(m_handle);
        }
    }

    counting_semaphore(counting_semaphore&& other) noexcept
        : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    counting_semaphore& operator=(counting_semaphore&& other) noexcept {
        counting_semaphore(std::move(other)).swap(*this);
        return *this;
    }

    counting_semaphore(const counting_semaphore&)            = delete;
    counting_semaphore& operator=(const counting_semaphore&) = delete;

    inline void swap(counting_semaphore& other) noexcept {
        using std::swap;
        swap(m_handle, other.m_handle);
    }

    inline void release(count_type update = 1) noexcept { give(update); }
    inline void aquire() { take(freertos::infinity_delay); }
    [[nodiscard]] inline bool try_aquire() { return take(0); }

    template <class REP_T, class PERIOD_T>
    [[nodiscard]] inline bool try_aquire_for(
        const std::chrono::duration<REP_T, PERIOD_T>& time) noexcept {
        return take(freertos::to_ticks(time));
    }

private:
    inline bool take(TickType_t ticks) noexcept {
        if (!this_cpu::is_in_isr()) {
            return (::xSemaphoreTake(m_handle, ticks) == pdTRUE);
        }

        ::BaseType_t needs_yield = pdFALSE;
        const bool success =
            (::xSemaphoreTakeFromISR(m_handle, &needs_yield) == pdTRUE);
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    inline bool give(count_type update) noexcept {
        if (!this_cpu::is_in_isr()) {
            while ((update > 0)) {
                if (::xSemaphoreGive(m_handle) != pdPASS) {
                    return false;
                }
                update--;
            }
            return true;
        }

        ::BaseType_t needs_yield = pdFALSE;
        bool success             = true;
        while (success && (update > 0)) {
            success =
                (::xSemaphoreGiveFromISR(m_handle, &needs_yield) == pdTRUE);
            update--;
        }
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    native_handle_type m_handle{nullptr};
};

using binary_semaphore = counting_semaphore<1>;

}  // namespace dis
#endif  // DIS_OSAL_THREAD_SEMAPHORE_HPP
