idf_component_register(SRCS "../src/esp32_nat_router.c"
                    INCLUDE_DIRS "." "../src/include" # <--- Añadimos la ruta aquí
                    REQUIRES wifi_handler web_server router_handler utils console

