qt_add_qml_module(ahora_app_qrcode
    URI Ahora_app_qrcode
    VERSION "1.0"
    SOURCES
        include/qrgen.h
        src/qrgen.cpp
        ../../external/qrcode/QR-Code-generator/cpp/qrcodegen.cpp
)

target_link_libraries(ahora_app_qrcode PRIVATE Qt6::Gui Qt6::Quick)

target_include_directories(ahora_app_qrcode PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../external/qrcode
)

