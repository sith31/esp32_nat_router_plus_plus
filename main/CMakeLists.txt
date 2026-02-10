idf_component_register(SRCS "nat_router.c" "web_server.c" "request_handler.c" "auth_handler.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_http_server fatfs sdmmc driver)

# Añade esta línea justo debajo para exponer los .h de main
idf_component_get_property(main_lib main COMPONENT_LIB)
target_include_directories(${main_lib} PUBLIC ".")

                    

target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/index.html.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/settings.html.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/info.html.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/error_404.html.gz" TEXT)

target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/main.css.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/dark.css.gz" TEXT)

target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/scan.js.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/settings.js.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/info.js.gz" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../components/web_server/www/output/gzip/functions.js.gz" TEXT)
