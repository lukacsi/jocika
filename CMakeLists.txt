cmake_minimum_required(VERSION 3.22)

project(jocika VERSION 0.1 DESCRIPTION "A discord bot")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(DPP REQUIRED)
find_package(MPG123 REQUIRED)
find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
#find_package(Opus REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    ${DPP_LIBRARIES}
    ${MPG123_LIBRARIES}
    ${CURL_LIBRARIES}
    SQLite::SQLite3
    #${OPUS_LIBRARY}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${DPP_INCLUDE_DIR}
    ${MPG123_INCLUDE_DIRS}
    #${OPUS_INCLUDE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
