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

#include "dis/osal/thread/thread.hpp"
#include "dis/osal/thread/mutex.hpp"
#include "dis/osal/thread/semaphore.hpp"

#include <Nucleo_F767ZI_GPIO.h>
#include <Nucleo_F767ZI_Init.h>
#include <stm32f7xx_hal.h>

// some common variables to use for each task
// 128 * 4 = 512 bytes
//(recommended min stack size per task)
#define STACK_SIZE 128

static void greenBlink(void);
static void blueTripleBlink(void);

void GreenTaskA(void* argument);
void TaskB(void* argumet);

// create storage for a pointer to a semaphore
dis::binary_semaphore global_semaphore(0);

int main(void) {
    HWInit();
    HAL_NVIC_SetPriorityGrouping(
        NVIC_PRIORITYGROUP_4);  // ensure proper priority grouping for freeRTOS

    // create TaskA as a higher priority than TaskB.  In this example, this
    // isn't strictly necessary since the tasks spend nearly all of their time
    // blocked
    assert_param(xTaskCreate(GreenTaskA, "GreenTaskA", STACK_SIZE, NULL,
                             tskIDLE_PRIORITY + 2, NULL) == pdPASS);

    // using an assert to ensure proper task creation
    assert_param(xTaskCreate(TaskB, "TaskB", STACK_SIZE, NULL,
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
void GreenTaskA(void* argument) {
    uint_fast8_t count = 0;

    while (1) {
        uint8_t numLoops = StmRand(3, 7);
        if (++count >= numLoops) {
            count = 0;
            global_semaphore.release();
        }
        greenBlink();
    }
}

/**
 * wait to receive semPtr and triple blink the Blue LED
 * If the semaphore isn't available within 500 mS, then
 * turn on the RED LED until the semaphore is available
 */
void TaskB(void* argument) {
    using namespace std::chrono_literals;
    const auto timeout = 500ms;

    while (1) {
        //'take' the semaphore with a 500mS timeout
        if (global_semaphore.try_aquire_for(timeout)) {
            RedLed.Off();
            blueTripleBlink();
        } else {
            // this code is called when the semaphore wasn't taken in time
            RedLed.On();
        }
    }
}

/**
 * Blink the Green LED once using vTaskDelay
 */
static void greenBlink(void) {
    using namespace std::chrono_literals;
    const auto delay = 100ms;

    GreenLed.On();
    dis::this_thread::sleep_for(delay);
    GreenLed.Off();
    dis::this_thread::sleep_for(delay);
}

/**
 * blink the Blue LED 3 times in rapid succession
 * using vtaskDelay
 */
static void blueTripleBlink(void) {
    using namespace std::chrono_literals;
    const auto delay = 50ms;
    // triple blink the Blue LED
    for (uint_fast8_t i = 0; i < 3; i++) {
        BlueLed.On();
        dis::this_thread::sleep_for(delay);
        BlueLed.Off();
        dis::this_thread::sleep_for(delay);
    }
}
