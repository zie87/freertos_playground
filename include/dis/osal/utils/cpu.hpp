#ifndef DIS_OSAL_UTILS_CPU_HPP
#define DIS_OSAL_UTILS_CPU_HPP

#include <FreeRTOS.h>

namespace dis::this_cpu {

[[nodiscard]] inline bool is_in_isr() noexcept {
    return (::xPortIsInsideInterrupt() == pdTRUE);
}

}  // namespace dis::this_cpu

#endif  // DIS_OSAL_UTILS_CPU_HPP
