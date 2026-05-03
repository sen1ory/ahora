# requirements ------------------------------------------------------------ {{{
cmake_minimum_required(VERSION 3.16)

project(ahora VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)
qt_standard_project_setup(REQUIRES 6.5)
# }}}
# subdirectories ------------------------------------------------------------ {{{
add_subdirectory(app/main)
add_subdirectory(app/qrcode)
add_subdirectory(core/ipadress)
# }}}
