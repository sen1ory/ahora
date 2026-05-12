qt_add_qml_module(ahora_app_main
    URI Ahora_app_main
    VERSION "1.0"
    QML_FILES
        qml/Main.qml
        qml/AdminPanel.qml
        qml/TeamCard.qml
        qml/GruvButton.qml
        qml/AhoraTheme.qml
)

add_executable(ahora
    src/main.cpp
)
target_include_directories(ahora PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../qrcode/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/httpserver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/wsserver/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../core/session/include
)
target_link_libraries(ahora PRIVATE 
    Qt6::Quick
    Qt6::Network
    Qt6::WebSockets
    ahora_app_main
    ahora_app_mainplugin_init
    ahora_app_mainplugin
    ahora_app_qrcode
    ahora_app_qrcodeplugin_init
    ahora_app_qrcodeplugin
    ahora_core_session
    ahora_core_wsserver
    ahora_core_httpserver
    ahora_core_ipaddress
    ahora_core_ipaddressplugin_init
    ahora_core_ipaddressplugin
)

qt_import_qml_plugins(ahora)
