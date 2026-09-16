#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "MAIN_APP";

// Handle for FreeRTOS
QueueHandle_t sensor_queue;
SemaphoreHandle_t stdout_mutex;

// Data structure of transmission between tasks
typedef struct {
    float temperature;
    uint32_t timestamp;
} sensor_data_t;

// TASK 1: Sensor reading simulation(low priority)
void task_sensor_read(void *pvParameters) {
    sensor_data_t data;
    float dummy_temp = 25.0;

    while (1) {
        // Temperature cycling simulation
        dummy_temp += 0.5;
        if (dummy_temp > 35.0) dummy_temp = 25.0;

        data.temperature = dummy_temp;
        data.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;

        // Send data to the queque(wait up to 100ms if the queque is full)
        if (xQueueSend(sensor_queue, &data, pdMS_TO_TICKS(100)) == pdPASS) {
            xSemaphoreTake(stdout_mutex, portMAX_DELAY);
            ESP_LOGI(TAG, "Sensor task: data sent to the queque -> Temp: %.2f C", data.temperature);
            xSemaphoreGive(stdout_mutex);
        }

        // Run this task every 2 seconds
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Task 2: data&log processing(higher priority)
void task_data_processor(void *pvParameters) {
    sensor_data_t received_data;

    while (1) {
        // Tunggu data dari Queue secara memblokir (indefinite wait
        if (xQueueReceive(sensor_queue, &received_data, portMAX_DELAY) == pdPASS) {
            
            xSemaphoreTake(stdout_mutex, portMAX_DELAY);
            ESP_LOGW(TAG, "Processor Task: Menerima data!");
            
            // Logika bisnis industri: Berikan peringatan jika suhu terlalu tinggi
            if (received_data.temperature > 30.0) {
                ESP_LOGE(TAG, "[ALERT] Suhu Terlalu Panas: %.2f C pada %ld ms", 
                         received_data.temperature, received_data.timestamp);
            } else {
                ESP_LOGI(TAG, "[NORMAL] Status Aman: %.2f C", received_data.temperature);
            }
            xSemaphoreGive(stdout_mutex);
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Memulai Sistem Monitoring IoT...");

    // 1. Inisialisasi Mutex untuk mengamankan Logging/Stdout
    stdout_mutex = xSemaphoreCreateMutex();

    // 2. Inisialisasi Queue (Kapasitas menampung 5 data struktur)
    sensor_queue = xQueueCreate(5, sizeof(sensor_data_t));

    if (sensor_queue != NULL && stdout_mutex != NULL) {
        
        // 3. Membuat Task dan menempatkannya ke Core yang spesifik (Core 1 untuk aplikasi)
        xTaskCreatePinnedToCore(
            task_sensor_read,       // Fungsi task
            "Sensor_Read_Task",     // Nama teks task (untuk debugging)
            3072,                   // Ukuran Stack (bytes)
            NULL,                   // Parameter yang dikirim
            2,                      // Prioritas Task
            NULL,                   // Task handle
            1                       // Jalankan di Core 1
        );

        xTaskCreatePinnedToCore(
            task_data_processor,
            "Data_Processor_Task",
            3072,
            NULL,
            3,                      // Prioritas lebih tinggi agar responsif terhadap data baru
            NULL,
            1                       // Jalankan di Core 1
        );
        
    } else {
        ESP_LOGE(TAG, "Gagal menginisialisasi komponen FreeRTOS!");
    }
}
