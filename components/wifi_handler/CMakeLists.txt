idf_component_register(SRCS "wifi_init.c" "wifi_handler.c" "wifi_event_handler.c"
                    INCLUDE_DIRS "." "../../main"  # <--- Añadimos esto para que encuentre router_globals.h
                    REQUIRES esp_wifi esp_event esp_netif esp_eth json)
