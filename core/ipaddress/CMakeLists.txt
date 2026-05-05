qt_add_qml_module(ahora_core_ipaddress
    URI Ahora_core_ipaddress
    VERSION "1.0"
    SOURCES
        include/ipaddress.h
        src/ipaddress.cpp
)

target_link_libraries(ahora_core_ipaddress PRIVATE Qt6::Gui Qt6::Quick Qt6::Network)

target_include_directories(ahora_core_ipaddress PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)