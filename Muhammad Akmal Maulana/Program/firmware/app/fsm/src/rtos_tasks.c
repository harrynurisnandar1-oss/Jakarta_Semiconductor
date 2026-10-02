/**
* @file rtos_tasks.c
* @brief Implementasi Task & Inter-Task Communication FreeRTOS E3
*/
#include "rtos_tasks.h"
#include "tamper_manager.h"
#include "load_profile.h"
#include "display.h"
#include "nvram_storage.h"
#include <stdio.h>
#include <string.h>
/* Stub/Simulasi untuk PC (CTest) agar tidak mencari file header FreeRTOS asli */
#if defined(EMBEDDED_HARDWARE_TARGET) || defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
static QueueHandle_t xTamperQueue = NULL;
#else
#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS(ms) (ms)
#endif
static inline void vTaskDelay(uint32_t ticks) { (void)ticks; }
/* Ring Buffer Simulasi Queue di RAM PC */
static tamper_event_msg_t simulated_tamper_queue[TAMPER_QUEUE_MAX_ITEMS];
static size_t q_head = 0;
static size_t q_tail = 0;
static size_t q_count = 0;
#endif
/* Global Context Handlers */
static tamper_context_t g_tamper_ctx;
static load_profile_ctx_t g_load_profile_buf;
static display_context_t g_display_ctx;
void rtos_system_init(void) {
 /* 1. Inisialisasi Seluruh Modul Subsystem E3 */
 tamper_init(&g_tamper_ctx);
 load_profile_init(&g_load_profile_buf);
 display_init(&g_display_ctx, "530000000001", 2000);
 /* 2. Inisialisasi Queue FreeRTOS / Simulasi */
#if defined(EMBEDDED_HARDWARE_TARGET) || defined(USE_FREERTOS)
 xTamperQueue = xQueueCreate(TAMPER_QUEUE_MAX_ITEMS, sizeof(tamper_event_msg_t));
#else
 q_head = 0;
 q_tail = 0;
 q_count = 0;
 #endif
 /* 3. Memuat Snapshot NVRAM Terakhir dari Flash */
 nvram_load_tamper_log_snapshot(&g_tamper_ctx);

 printf("[RTOS] All E3 Firmware Subsystems, Queues & NVRAM Initialized Successfully!\n");
}
bool rtos_queue_send_tamper_event(const tamper_event_msg_t *msg) {
 if (msg == NULL) return false;
#if defined(EMBEDDED_HARDWARE_TARGET) || defined(USE_FREERTOS)
 if (xTamperQueue == NULL) return false;
 return (xQueueSend(xTamperQueue, msg, pdMS_TO_TICKS(10)) == pdPASS);
#else
 if (q_count >= TAMPER_QUEUE_MAX_ITEMS) return false; /* Queue Penuh */
 simulated_tamper_queue[q_head] = *msg;
 q_head = (q_head + 1) % TAMPER_QUEUE_MAX_ITEMS;
 q_count++;
 return true;
#endif
}
bool rtos_queue_receive_tamper_event(tamper_event_msg_t *out_msg, uint32_t timeout_ms) {
 (void)timeout_ms;
 if (out_msg == NULL) return false;
#if defined(EMBEDDED_HARDWARE_TARGET) || defined(USE_FREERTOS)
 if (xTamperQueue == NULL) return false;
 return (xQueueReceive(xTamperQueue, out_msg, pdMS_TO_TICKS(timeout_ms)) == pdPASS);
#else
 if (q_count == 0) return false; /* Queue Kosong */
 *out_msg = simulated_tamper_queue[q_tail];
 q_tail = (q_tail + 1) % TAMPER_QUEUE_MAX_ITEMS;
 q_count--;
return true;
#endif
}
void task_tamper_emergency_entry(void *pvParameters) {
 (void)pvParameters;
 /* Task Darurat Sabotase (Priority 4 - High) */
 for (;;) {
 vTaskDelay(pdMS_TO_TICKS(1000));
 }
}
void task_metrology_profiling_entry(void *pvParameters) {
 (void)pvParameters;
 /* Task Pembekuan Profil Beban 15-Menit (Priority 3 - Medium) */
 for (;;) {
 vTaskDelay(pdMS_TO_TICKS(15000));
 }
}
void task_dlms_entry(void *pvParameters) {
 (void)pvParameters;
 /* Task Protokol DLMS/COSEM (Priority 2 - Normal) */
 tamper_event_msg_t event_msg;
 for (;;) {
 /* Membaca event sabotase dari Queue */
 if (rtos_queue_receive_tamper_event(&event_msg, 100)) {
 printf("[DLMS TASK] Tamper Alert Diterima! Code: 0x%02X, Time: %u. Menyiapkan DLMS Event Push...\n",
 event_msg.tamper_code, event_msg.timestamp);
 }
 vTaskDelay(pdMS_TO_TICKS(500));
}
}
void task_ui_display_entry(void *pvParameters) {
 (void)pvParameters;
 /* Task Tampilan UI / LCD Carousel (Priority 1 - Low) */
 for (;;) {
 display_process_tick(&g_display_ctx, 2000);
 vTaskDelay(pdMS_TO_TICKS(2000));
 }
}
