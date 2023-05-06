#ifndef DIS_OSAL_THREAD_MUTEX_HPP
#define DIS_OSAL_THREAD_MUTEX_HPP

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/utils/cpu.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

#include <utility>

namespace dis {

class mutex {
public:
    using native_handle_type = ::SemaphoreHandle_t;

    mutex() noexcept : m_handle{xSemaphoreCreateMutex()} {}
    ~mutex() noexcept { if(m_handle) { ::vSemaphoreDelete(m_handle); } }

    mutex(mutex&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    mutex& operator=(mutex&& other) noexcept {
        mutex(std::move(other)).swap(*this);
        return *this;
    }

    mutex(const mutex&)            = delete;
    mutex& operator=(const mutex&) = delete;

    inline void lock() noexcept { take(freertos::infinity_delay); }
    [[nodiscard]] inline bool try_lock() noexcept { return take(0); }

    template <class REP_T, class PERIOD_T>
    [[nodiscard]] inline bool try_lock_for(
        const std::chrono::duration<REP_T, PERIOD_T>& time) noexcept {
        return take(freertos::to_ticks(time));
    }

    inline void unlock() noexcept { give(); }

    void swap(mutex& other) noexcept {
        using std::swap;
        swap(m_handle, other.m_handle);
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

    inline bool give() noexcept {
        if (!this_cpu::is_in_isr()) {
            return (::xSemaphoreGive(m_handle) == pdPASS);
        }

        ::BaseType_t needs_yield = pdFALSE;
        const bool success =
            (::xSemaphoreGiveFromISR(m_handle, &needs_yield) == pdTRUE);
        portYIELD_FROM_ISR(needs_yield);
        return success;
    }

    native_handle_type m_handle{nullptr};
};

}  // namespace dis

#endif  // DIS_OSAL_THREAD_MUTEX_HPP
