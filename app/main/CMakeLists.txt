qt_add_qml_module(ahora_app_main
    URI Ahora_app_main
    VERSION "1.0"
    QML_FILES
        qml/Main.qml

)

add_executable(ahora
    src/main.cpp
)
target_link_libraries(ahora PRIVATE 
    Qt6::Quick
    Qt6::Network
ahora_app_main
)
