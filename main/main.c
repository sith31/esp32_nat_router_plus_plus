#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/* Header principal del router */
#include "esp32_nat_router.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting ESP32 NAT Router ++");

    /* Inicializacion principal del router */
    esp32_nat_router_init();

    /* Nada mas que hacer aqui, el router crea sus tareas */
}
