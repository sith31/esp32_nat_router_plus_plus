#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/* Incluimos directamente el codigo del router */
#include "../src/esp32_nat_router.c"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ESP32 NAT Router ++");

    /* Si el router ya tiene app_main interno, NO hagas nada mas */
    /* Si no, aqui se ejecutara el codigo incluido */
}
