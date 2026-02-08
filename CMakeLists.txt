# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
idf_component_register(SRCS "esp_nat_router.c" 
                            "web_server.c" 
                            "request_handler.c" 
                            "auth_handler.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_http_server fatfs sdmmc esp_driver_gpio esp_driver_sdmmc)
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_nat_router+)
