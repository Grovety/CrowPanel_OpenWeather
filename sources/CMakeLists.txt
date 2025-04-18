file(GLOB_RECURSE USECASES_SRC "usecases/*.cpp")
file(GLOB_RECURSE ADAPTERS_SRC "adapters/*.cpp")
file(GLOB_RECURSE ENTITIES_SRC "entities/*.cpp")
file(GLOB_RECURSE IMAGES_SRC_BASE "images/*.c")
file(GLOB_RECURSE IMAGES_SRC_DARK  "images/dark_theme/*.c")
set(IMAGES_SRC ${IMAGES_SRC_BASE} ${IMAGES_SRC_DARK})

list(REMOVE_ITEM IMAGES_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/images/13d.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/13n.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/50d.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/50n.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/humidity.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/pressure.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/wind.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/settings.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/wifi_100.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/wifi_75.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/wifi_50.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/images/wifi_25.c"
)

idf_component_register(SRCS "main.cpp" ${USECASES_SRC} ${ADAPTERS_SRC} ${ENTITIES_SRC} ${IMAGES_SRC}
                    REQUIRES spi_flash esp32_display_panel esp_timer lvgl esp_wifi esp_http_client json_parser nvs_flash lwip
                    INCLUDE_DIRS "." "usecases" "adapters" "entities")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
# The following code is to avoid the error:
# lvgl_v8_port/managed_components/lvgl__lvgl/demos/stress/lv_demo_stress.c:92:29: error: format '%d' expects argument of
# type 'int', but argument 6 has type 'uint32_t' {aka 'long unsigned int'} [-Werror=format=]

# Get the exact component name
idf_build_get_property(build_components BUILD_COMPONENTS)
foreach(COMPONENT ${build_components})
    if(COMPONENT MATCHES "lvgl" OR COMPONENT MATCHES "lvgl__lvgl")
        set(TARGET_COMPONENT ${COMPONENT})
        break()
    endif()
endforeach()
# Get the component library
if(TARGET_COMPONENT STREQUAL "")
    message(FATAL_ERROR "Component 'lvgl' not found.")
else()
    idf_component_get_property(LVGL_LIB ${TARGET_COMPONENT} COMPONENT_LIB)
endif()
target_compile_options(${LVGL_LIB} PRIVATE "-Wno-format")
set(TARGET_COMPONENT "")
                    