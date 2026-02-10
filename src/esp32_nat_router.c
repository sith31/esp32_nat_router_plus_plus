#include <stdio.h>
#include <esp_system.h>
#include <esp_log.h>
#include <lwip/lwip_napt.h>
#include "esp_netif.h"

#include "cmd_decl.h"
#include "router_globals.h"
#include "get_data_handler.h"
#include "auth_handler.h"
#include "initialization.h"
#include "hardware_handler.h"
#include "web_server.h"
#include "console_handler.h"
#include "file_system.h"
#include "mac_generator.h"
#include "nvm.h"
#include "router_handler.h"
#include "wifi_init.h"
#include "ota_handler.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#if !IP_NAPT
#error "IP_NAPT must be defined"
#endif

/* Global vars */
uint16_t connect_count = 0;
bool ap_connect = false;

uint32_t my_ip;
uint32_t my_ap_ip;

static const char *TAG = "ESP32 NAT router +";

void init_sd_card() {
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    
    // Pines para ESP32-S3 (Ajusta si tu placa usa otros)
    slot_config.width = 1; 
    slot_config.clk = GPIO_NUM_39; 
    slot_config.cmd = GPIO_NUM_38;
    slot_config.d0  = GPIO_NUM_40;

    ESP_LOGI("SD", "Montando tarjeta SD...");
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE("SD", "Error al montar SD (%s)", esp_err_to_name(ret));
    }
}


//-----------------------------------------------------------------------------
void app_main(void)
{
    initialize_nvs();
    init_sd_card();
    ip_napt_enable(_ip, 1);

#if CONFIG_STORE_HISTORY
    initialize_filesystem();
    ESP_LOGI(TAG, "Command history enabled");
#else
    ESP_LOGI(TAG, "Command history disabled");
#endif

    ESP_ERROR_CHECK(parms_init());
    hardware_init();
    get_portmap_tab();
    wifi_init();
    ip_napt_enable(my_ap_ip, 1);
    ESP_LOGI(TAG, "NAT is enabled");

    if (IsWebServerEnable)
    {
        ESP_LOGI(TAG, "Starting config web server");
        server = start_webserver();
    }

    ota_update_init();
    //initialize_console();
    register_system();
    register_nvs();
    register_router();
    //start_console();
    ESP_LOGI(TAG, "Sistema listo y NAT activo");
}

//-----------------------------------------------------------------------------
