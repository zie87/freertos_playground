/**
 * MIT License
 *
 * Copyright (c) 2019 Brian Amos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/thread/mutex.hpp"

#include <Nucleo_F767ZI_GPIO.h>
#include <Nucleo_F767ZI_Init.h>
#include <stm32f7xx_hal.h>

// some common variables to use for each task
// 128 * 4 = 512 bytes
//(recommended min stack size per task)
#define STACK_SIZE 128

static void blinkTwice(LED* led);
static void lookBusy(uint32_t numIterations);

void TaskA(void* argument);
void TaskB(void* argumet);
void TaskC(void* argumet);

// create storage for a pointer to a mutex (this is the same container as a
// semaphore)
dis::mutex global_mutex{};

int main(void) {
    HWInit();
    HAL_NVIC_SetPriorityGrouping(
        NVIC_PRIORITYGROUP_4);  // ensure proper priority grouping for freeRTOS

    assert_param(xTaskCreate(TaskA, "TaskA", STACK_SIZE, NULL,
                             tskIDLE_PRIORITY + 3, NULL) == pdPASS);
    assert_param(xTaskCreate(TaskB, "TaskB", STACK_SIZE, NULL,
                             tskIDLE_PRIORITY + 2, NULL) == pdPASS);
    assert_param(xTaskCreate(TaskC, "TaskC", STACK_SIZE, NULL,
                             tskIDLE_PRIORITY + 1, NULL) == pdPASS);

    // start the scheduler - shouldn't return unless there's a problem
    vTaskStartScheduler();

    // if you've wound up here, there is likely an issue with overrunning the
    // freeRTOS heap
    while (1) {
    }
}

/**
 * Task A periodically 'gives' semaphorePtr.  This version
 * has some variability in how often it will give the semaphore
 * NOTES:
 * - This semaphore isn't "given" to any task specifically
 * - giving the semaphore doesn't prevent taskA from continuing to run.
 *   Notice the green LED continues to blink at all times
 */
void TaskA(void* argument) {
    using namespace std::chrono_literals;
    const auto timeout = 200ms;

    while (true) {
        if (global_mutex.try_lock_for(timeout)) {
            RedLed.Off();
            blinkTwice(&GreenLed);
            global_mutex.unlock();
        } else {
            RedLed.On();
        }
        // sleep for a bit to let other tasks run
        dis::this_thread::sleep_for(std::chrono::milliseconds{StmRand(5, 30)});
    }
}

/**
 * this task just wakes up periodically and wastes time.
 */
void TaskB(void* argument) {
    uint32_t counter = 0;
    while (1) {
        dis::this_thread::sleep_for(std::chrono::milliseconds{StmRand(10, 25)});
        lookBusy(StmRand(250000, 750000));
    }
}

/**
 * wait to receive semPtr and double blink the Blue LED
 * If the semaphore isn't available within 500 mS, then
 * turn on the RED LED until the semaphore is available
 */
void TaskC(void* argument) {
    using namespace std::chrono_literals;
    const auto timeout = 200ms;

    while (true) {
        //'take' the semaphore with a 200mS timeout
        if (global_mutex.try_lock_for(timeout)) {
            RedLed.Off();
            blinkTwice(&BlueLed);
            global_mutex.unlock();
        } else {
            // this code is called when the semaphore wasn't taken in time
            RedLed.On();
        }
    }
}

/**
 * Blink the desired LED twice
 */
static void blinkTwice(LED* led) {
    for (uint32_t i = 0; i < 2; i++) {
        using namespace std::chrono_literals;
        led->On();
        dis::this_thread::sleep_for(43ms);
        led->Off();
        dis::this_thread::sleep_for(43ms);
    }
}

/**
 * run a simple loop for numIterations
 * @param numIterations number of iterations to compute modulus
 */
static void lookBusy(uint32_t numIterations) {
    __attribute__((unused)) volatile uint32_t dontCare = 0;
    for (int i = 0; i < numIterations; i++) {
        dontCare = i % 4;
    }
}
