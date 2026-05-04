# Модуль: WsServer — WebSocket-сервер для связи с клиентами
# Собирается как статическая библиотека

add_library(ahora_core_wsserver STATIC
    include/WsServer.h
    src/WsServer.cpp
)

target_link_libraries(ahora_core_wsserver PUBLIC
    Qt6::Core
    Qt6::WebSockets
    ahora_core_session
)

target_include_directories(ahora_core_wsserver PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
