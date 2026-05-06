add_executable(test_sessionmanager
    test_sessionmanager.cpp
)

target_link_libraries(test_sessionmanager PRIVATE
    Qt6::Test
    Qt6::Quick
    Qt6::WebSockets
    ahora_core_session
)

target_include_directories(test_sessionmanager PRIVATE
    ${CMAKE_SOURCE_DIR}/core/session/include
)

add_test(NAME SessionManagerTests COMMAND test_sessionmanager)


add_executable(test_quizanswer
    test_quizanswer.cpp
)

target_link_libraries(test_quizanswer PRIVATE
    Qt6::Test
    ahora_core_session
)

target_include_directories(test_quizanswer PRIVATE
    ${CMAKE_SOURCE_DIR}/core/session/include
)

add_test(NAME QuizAnswerTests COMMAND test_quizanswer)
