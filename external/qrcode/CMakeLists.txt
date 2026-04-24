cmake_minimum_required(VERSION 3.16)
project(qrcode_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

add_executable(qrcode_demo
    main.cpp
    qrgen.cpp
    qrgen.h
    QR-Code-generator/cpp/qrcodegen.cpp
)

target_include_directories(qrcode_demo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/QR-Code-generator/cpp
)

target_link_libraries(qrcode_demo Qt6::Quick)
