
file(GLOB SOURCES ./*.c tools/*.c tools/*.cpp include/*.c)

idf_component_register(
        SRCS
        SRCS ${SOURCES}
        "ESP_WIFI_TOOLS.c"
        INCLUDE_DIRS
        "."
)
