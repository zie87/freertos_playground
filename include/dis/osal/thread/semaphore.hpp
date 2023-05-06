#ifndef DIS_OSAL_THREAD_SEMAPHORE_HPP
#define DIS_OSAL_THREAD_SEMAPHORE_HPP

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/thread/detail/semaphore_policy.hpp"
#include "dis/osal/utils/cpu.hpp"

#include <FreeRTOS.h>
#include <semphr.h>

namespace dis {

template <std::ptrdiff_t LEAST_MAX_V>
class counting_semaphore {
    using policy_type = detail::semaphore_policy;

public:
    using count_type         = typename policy_type::count_type;
    using native_handle_type = typename policy_type::native_handle_type;

    counting_semaphore(count_type desired = 0) noexcept
        : m_handle{policy_type::create<LEAST_MAX_V>(desired)} {}

    ~counting_semaphore() noexcept { policy_type::destroy(m_handle); }

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

    inline void release(count_type update = 1) noexcept {
        policy_type::give(m_handle, update);
    }
    inline void aquire() {
        policy_type::take(m_handle, freertos::infinity_delay);
    }
    [[nodiscard]] inline bool try_aquire() {
        return policy_type::take(m_handle, 0);
    }

    template <class REP_T, class PERIOD_T>
    [[nodiscard]] inline bool try_aquire_for(
        const std::chrono::duration<REP_T, PERIOD_T>& time) noexcept {
        return policy_type::take(m_handle, freertos::to_ticks(time));
    }

private:
    native_handle_type m_handle{nullptr};
};

using binary_semaphore = counting_semaphore<1>;

}  // namespace dis
#endif  // DIS_OSAL_THREAD_SEMAPHORE_HPP
