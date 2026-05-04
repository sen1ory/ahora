# Модуль: HttpServer — минимальный HTTP-сервер для статики
# Собирается как статическая библиотека

add_library(ahora_core_httpserver STATIC
    include/HttpServer.h
    src/HttpServer.cpp
    src/web_content.h
)

target_link_libraries(ahora_core_httpserver PUBLIC
    Qt6::Core
    Qt6::Network
)

target_include_directories(ahora_core_httpserver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
