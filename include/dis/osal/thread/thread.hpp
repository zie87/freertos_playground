#ifndef DIS_OSAL_THREAD_THREAD_HPP
#define DIS_OSAL_THREAD_THREAD_HPP

#include "dis/osal/debug/contracts.hpp"

#include <FreeRTOS.h>
#include <task.h>

#include <chrono>

namespace dis {
namespace freertos {
constexpr TickType_t infinity_delay = portMAX_DELAY;
constexpr TickType_t tick_rate_ms   = portTICK_RATE_MS;
inline constexpr TickType_t to_ticks(std::chrono::milliseconds time) noexcept {
    const auto ticks = (time.count() * configTICK_RATE_HZ) / 1000;
    return {ticks};
}
}  // namespace freertos

namespace this_thread {

void sleep_for(const std::chrono::milliseconds& sleep) noexcept {
    const auto ticks = freertos::to_ticks(sleep);
    ::vTaskDelay(ticks);
}

template <class REP_T, class PERIOD_T>
void sleep_for(const std::chrono::duration<REP_T, PERIOD_T>& sleep) noexcept {
    const auto sleep_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(sleep);
    sleep_for(sleep_ms);
}
}  // namespace this_thread
}  // namespace dis

#endif  // DIS_OSAL_THREAD_THREAD_HPP
