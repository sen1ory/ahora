qt_add_qml_module(ahora_core_ipadress
    URI Ahora_core_ipadress
    VERSION "1.0"
    SOURCES
        include/ipadress.h
        src/ipadress.cpp
)

target_link_libraries(ahora_core_ipadress PRIVATE Qt6::Gui Qt6::Quick Qt6::Network)

target_include_directories(ahora_core_ipadress PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)