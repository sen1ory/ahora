# Модуль: SessionManager — модель команд для QML
# Собирается как статическая библиотека, подключается к главному исполняемому файлу

add_library(ahora_core_session STATIC
    include/SessionManager.h
    include/QuizDef.h
    src/SessionManager.cpp
)

target_link_libraries(ahora_core_session PUBLIC
    Qt6::Core
    Qt6::Quick
    Qt6::WebSockets
)

target_include_directories(ahora_core_session PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
