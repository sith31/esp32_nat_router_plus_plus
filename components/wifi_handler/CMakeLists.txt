idf_component_register(SRCS "wifi_init.c" "wifi_handler.c" "wifi_event_handler.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_event esp_netif esp_eth)
