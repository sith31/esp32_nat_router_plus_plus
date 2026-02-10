idf_component_register(
    SRCS console_handler.c file_system.c
    INCLUDE_DIRS include
    REQUIRES
        cmd_router
        cmd_nvs
        nvs_flash
        fatfs
        json
        vfs
        esp_driver_usb_serial_jtag
)

                    

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
