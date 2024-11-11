find_path(MPG123_INCLUDE_DIR
    NAMES mpg123.h
    PATHS /usr/include /usr/local/include
)

find_library(MPG123_LIBRARY
    NAMES mpg123
    PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MPG123 DEFAULT_MSG MPG123_LIBRARY MPG123_INCLUDE_DIR)

if(MPG123_FOUND)
    set(MPG123_LIBRARIES ${MPG123_LIBRARY})
    set(MPG123_INCLUDE_DIRS ${MPG123_INCLUDE_DIR})
endif()
