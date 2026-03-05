/*
    author: Conbiao
    date: 2026/02/28
    
    This is the code of the ICM-42688P app.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "icm42688p.h"

static const char *APP_TAG = "MAIN_APP";

void icm_data_read_task(void *pvParameters)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    while (1) 
    {
        icm42688p_get_accel_data(&ax, &ay, &az);
        icm42688p_get_gyro_data(&gx, &gy, &gz);
        
        ESP_LOGI(APP_TAG, "-----------------------------------------");
        ESP_LOGI(APP_TAG, "Accel [X:%.2f Y:%.2f Z:%.2f] g", accel_x_g, accel_y_g, accel_z_g);
        ESP_LOGI(APP_TAG, "Gyro  [X:%.2f Y:%.2f Z:%.2f] rad/s", gyro_x_rad, gyro_y_rad, gyro_z_rad);

        vTaskDelay(pdMS_TO_TICKS(100)); //10hz = 100ms
    }
}

void app_main(void)
{
    ESP_LOGI(APP_TAG, "Initializing ICM-42688P...");

    icm42688p_init();

    ESP_LOGI(APP_TAG, "Initialization complete. Starting data collection...");

    xTaskCreate(icm_data_read_task, "icm_read_task", 4096, NULL, 5, NULL);

    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}