project(
    'stm32_thread',
    ['c', 'cpp'],
    version: '0.1',
    default_options: ['c_std=gnu11', 'cpp_std=c++20', 'warning_level=3'],
)

stm32_thread_linker_script = join_paths(
    meson.current_source_dir(),
    'STM32F767ZI_FLASH.ld',
)
stm32_thread_startup_file = join_paths(
    'startup',
    'startup_stm32f767xx.s',
)
stm32_thread_link_args = ['-T' + stm32_thread_linker_script]

config_inc_dirs = include_directories('include')

bsp_dir = join_paths('BSP')
bsp_inc_dirs = include_directories(bsp_dir)

cmsis_dir = join_paths('Drivers', 'CMSIS')
cmsis_inc_dirs = include_directories(
    join_paths(cmsis_dir, 'Include'),
    join_paths(cmsis_dir, 'Device', 'ST', 'STM32F7xx', 'Include'),
)
cmsis_c_args = ['-DUSE_HAL_DRIVER']

hal_dir = join_paths(
    'Drivers',
    'STM32F7xx_HAL_Driver',
)
hal_inc_dirs = include_directories(
    join_paths(hal_dir, 'Inc'),
    join_paths(hal_dir, 'Inc', 'Legacy'),
)
hal_c_args = ['-DSTM32F767xx']
hal_srcs = [
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_cortex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_dma.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_dma_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_eth.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_exti.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_flash.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_flash_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_gpio.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_i2c.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_i2c_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_pcd.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_pcd_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_pwr.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_pwr_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_rcc.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_rcc_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_tim.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_tim_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_uart.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_hal_uart_ex.c'),
    join_paths(hal_dir, 'Src', 'stm32f7xx_ll_usb.c'),
    join_paths(hal_dir, 'Inc', 'stm32f7xx_hal_adc.c'),
    join_paths(hal_dir, 'Inc', 'stm32f7xx_hal_adc_ex.c'),
]

hal_lib = library(
    'stm32_hal',
    sources: hal_srcs,
    include_directories: [cmsis_inc_dirs, hal_inc_dirs, config_inc_dirs],
    c_args: [cmsis_c_args, hal_c_args],
)

hal_dep = declare_dependency(
    link_with: hal_lib,
    include_directories: [cmsis_inc_dirs, hal_inc_dirs, config_inc_dirs],
    compile_args: [cmsis_c_args, hal_c_args],
)

third_party_dir = join_paths(
    'Middleware',
    'Third_Party',
)

freertos_dir = join_paths(third_party_dir, 'FreeRTOS')
freertos_inc_dirs = include_directories(
    join_paths(freertos_dir, 'Source', 'include'),
    join_paths(freertos_dir, 'Source', 'CMSIS_RTOS_V2'),
    join_paths(freertos_dir, 'Source', 'portable', 'GCC', 'ARM_CM7', 'r0p1'),
)

freertos_srcs = [
    join_paths(freertos_dir, 'Source', 'portable', 'MemMang', 'heap_4.c'),
    join_paths(
        freertos_dir,
        'Source',
        'portable',
        'GCC',
        'ARM_CM7',
        'r0p1',
        'port.c',
    ),
    join_paths(freertos_dir, 'Source', 'CMSIS_RTOS_V2', 'cmsis_os2.c'),
    join_paths(freertos_dir, 'Source', 'croutine.c'),
    join_paths(freertos_dir, 'Source', 'event_groups.c'),
    join_paths(freertos_dir, 'Source', 'list.c'),
    join_paths(freertos_dir, 'Source', 'queue.c'),
    join_paths(freertos_dir, 'Source', 'stream_buffer.c'),
    join_paths(freertos_dir, 'Source', 'tasks.c'),
    join_paths(freertos_dir, 'Source', 'timers.c'),
]

freertos_lib = library(
    'freertos',
    sources: freertos_srcs,
    include_directories: [freertos_inc_dirs, config_inc_dirs],
    dependencies: hal_dep,
)

freertos_dep = declare_dependency(
    link_with: freertos_lib,
    include_directories: [freertos_inc_dirs, config_inc_dirs],
    dependencies: hal_dep,
)

stm32_thread_inc_dirs = [
    include_directories('include'),
    config_inc_dirs,
    bsp_inc_dirs,
]
stm32_thread_c_args = []
stm32_thread_srcs = [
    stm32_thread_startup_file,
    'src/main.cpp',
    'src/stm32f7xx_hal_msp.c',
    'src/stm32f7xx_it.c',
    'src/syscalls.c',
    'src/system_stm32f7xx.c',

    join_paths(bsp_dir, 'Nucleo_F767ZI_GPIO.c'),
    join_paths(bsp_dir, 'Nucleo_F767ZI_Init.c'),
    join_paths(bsp_dir, 'UartQuickDirtyInit.c'),
]

stm32_thread = executable(
    'stm32_thread',
    sources: stm32_thread_srcs,
    include_directories: stm32_thread_inc_dirs,
    c_args: stm32_thread_c_args,
    link_args: stm32_thread_link_args,
    dependencies: [hal_dep, freertos_dep],
    name_suffix: 'elf',
)

objcopy = '@0@'.format(find_program('objcopy').path())
objdump = '@0@'.format(find_program('objdump').path())
size = '@0@'.format(find_program('size').path())

custom_target(
    'stm32_thread_size',
    input: stm32_thread,
    output: ['stm32_thread.size'],
    build_by_default: true,
    command: [size, '@INPUT@'],
    depends: [stm32_thread],
)

custom_target(
    'stm32_thread_list',
    input: stm32_thread,
    output: ['stm32_thread.list'],
    capture: true,
    build_by_default: true,
    command: [objdump, '-h', '-S', '@INPUT@'],
    depends: [stm32_thread],
)

custom_target(
    'stm32_thread_hex',
    input: stm32_thread,
    output: ['stm32_thread.hex'],
    build_by_default: true,
    command: [objcopy, '-O', 'ihex', '@INPUT@', '@OUTPUT@'],
    depends: [stm32_thread],
)

custom_target(
    'stm32_thread_bin',
    input: stm32_thread,
    output: ['stm32_thread.bin'],
    build_by_default: true,
    command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
    depends: [stm32_thread],
)
