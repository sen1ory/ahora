# Модуль: HttpServer — минимальный HTTP-сервер для статики
# Собирается как статическая библиотека

set(WEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web)
set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(GENERATED_HEADER ${GENERATED_DIR}/web_content.h)

# Build-time generation: runs every time web files change
add_custom_command(
    OUTPUT ${GENERATED_HEADER}
    DEPENDS
        ${WEB_DIR}/style.css
        ${WEB_DIR}/script.js
        ${WEB_DIR}/index.html
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generate_web_content.cmake
    COMMAND ${CMAKE_COMMAND}
        -D WEB_DIR=${WEB_DIR}
        -D OUTPUT_HEADER=${GENERATED_HEADER}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/src/generate_web_content.cmake
    COMMENT "Generating web_content.h from web client files"
)

add_library(ahora_core_httpserver STATIC
    include/HttpServer.h
    src/HttpServer.cpp
    ${GENERATED_HEADER}
)

target_include_directories(ahora_core_httpserver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${GENERATED_DIR}
)

target_link_libraries(ahora_core_httpserver PUBLIC
    Qt6::Core
    Qt6::Network
)
