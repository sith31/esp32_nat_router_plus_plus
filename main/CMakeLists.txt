idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        console_handler
        cmd_router
        cmd_nvs
)
