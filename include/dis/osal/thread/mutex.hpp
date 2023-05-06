#ifndef DIS_OSAL_THREAD_MUTEX_HPP
#define DIS_OSAL_THREAD_MUTEX_HPP

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/thread/detail/semaphore_policy.hpp"
#include "dis/osal/utils/cpu.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

#include <utility>

namespace dis {

class mutex {
    using policy_type = detail::semaphore_policy;

public:
    using native_handle_type = typename policy_type::native_handle_type;

    mutex() noexcept : m_handle{policy_type::create(detail::mutex_tag{})} {}
    ~mutex() noexcept { policy_type::destroy(m_handle); }

    mutex(mutex&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    mutex& operator=(mutex&& other) noexcept {
        mutex(std::move(other)).swap(*this);
        return *this;
    }

    mutex(const mutex&)            = delete;
    mutex& operator=(const mutex&) = delete;

    inline void lock() noexcept {
        policy_type::take(m_handle, freertos::infinity_delay);
    }
    [[nodiscard]] inline bool try_lock() noexcept {
        return policy_type::take(m_handle, 0);
    }

    template <class REP_T, class PERIOD_T>
    [[nodiscard]] inline bool try_lock_for(
        const std::chrono::duration<REP_T, PERIOD_T>& time) noexcept {
        return policy_type::take(m_handle, freertos::to_ticks(time));
    }

    inline void unlock() noexcept { policy_type::give(m_handle); }

    void swap(mutex& other) noexcept {
        using std::swap;
        swap(m_handle, other.m_handle);
    }

private:
    native_handle_type m_handle{nullptr};
};

}  // namespace dis

#endif  // DIS_OSAL_THREAD_MUTEX_HPP
