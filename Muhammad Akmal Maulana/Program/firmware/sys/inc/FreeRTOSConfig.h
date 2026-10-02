/**
 * @file FreeRTOSConfig.h
 * @brief Konfigurasi Kernel FreeRTOS untuk STM32U585 (Arm Cortex-M33 @ 160 MHz)
 * @details Disesuaikan dengan Laporan Rekayasa Master Smart Meter 3-Fasa (E3/ENG-3).
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>

/* --- System & Clock Settings --- */
#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1
#define configCPU_CLOCK_HZ                      ( 160000000UL ) /* 160 MHz System Clock */
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 ) /* 1 ms tick */
#define configMAX_PRIORITIES                    ( 7 )
#define configMINIMAL_STACK_DEPTH               ( ( uint16_t ) 128 )
#define configMAX_TASK_NAME_LEN                 ( 16 )
#define configUSE_TIME_SLICING                  1

/* --- Memory Allocation Configuration --- */
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 64 * 1024 ) ) /* 64 KB Heap */

/* --- Cortex-M33 / STM32U585 NVIC Priority --- */
#define configPRIO_BITS                         4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* --- Pemetaan Prioritas Task Sub-sistem 3 (Host MCU) --- */
#define TASK_PRIO_METROLOGY_READER    ( configMAX_PRIORITIES - 1 ) /* Prioritas 6 (Tinggi): ADE9078 SPI 1s */
#define TASK_PRIO_TAMPER_UI           ( configMAX_PRIORITIES - 2 ) /* Prioritas 5 (Sedang-Tinggi): LCD & EXTI IRQ */
#define TASK_PRIO_ACTUATION           ( configMAX_PRIORITIES - 2 ) /* Prioritas 5 (Sedang-Tinggi): Pulsa Relai */
#define TASK_PRIO_DLMS_COMM           ( configMAX_PRIORITIES - 3 ) /* Prioritas 4 (Normal): Engine DLMS/COSEM */
#define TASK_PRIO_PROFILING_LOGGER    ( configMAX_PRIORITIES - 4 ) /* Prioritas 3 (Rendah): Load Profile Flash 2MB */

#endif /* FREERTOS_CONFIG_H */