# Модуль: HttpServer — минимальный HTTP-сервер для статики
# Собирается как статическая библиотека

# Read web client source files and embed them into C++ header
set(WEB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/web)
file(READ ${WEB_DIR}/style.css WEB_STYLE_CONTENT)
file(READ ${WEB_DIR}/script.js WEB_SCRIPT_CONTENT)

# Regenerate when web files change
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${WEB_DIR}/style.css
    ${WEB_DIR}/script.js
)

# Generate C++ header with embedded web content
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/web_content.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/src/web_content.h
    @ONLY
)

add_library(ahora_core_httpserver STATIC
    include/HttpServer.h
    src/HttpServer.cpp
)

target_link_libraries(ahora_core_httpserver PUBLIC
    Qt6::Core
    Qt6::Network
)

target_include_directories(ahora_core_httpserver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
