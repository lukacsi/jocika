cmake_minimum_required(VERSION 3.22)

project(jocika VERSION 0.1 DESCRIPTION "A discord bot")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

find_package(DPP REQUIRED)
find_package(MPG123 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    ${DPP_LIBRARIES}
    ${MPG123_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${DPP_INCLUDE_DIR}
    ${MPG123_INCLUDE_DIRS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
