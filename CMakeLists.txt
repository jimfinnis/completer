cmake_minimum_required(VERSION 2.6)
project(COMPLETER)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Libedit REQUIRED)

add_executable(command_completer completer.cpp commandCompleterExample.cpp)
target_include_directories(command_completer PRIVATE ${LIBEDIT_INCLUDE_DIRS})
target_link_libraries(command_completer ${LIBEDIT_LIBRARIES})
