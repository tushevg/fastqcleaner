cmake_minimum_required(VERSION 3.5)

if(APPLE)
    # Fix linking on 10.14+ See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
endif()

project(fastqcleaner LANGUAGES C)

file(GLOB SOURCES *.c)
file(GLOB HEADERS *.h)
add_executable(fastqcleaner ${SOURCES} ${HEADERS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(ZLIB REQUIRED zlib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ZLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})

pkg_check_modules(HTSLIB REQUIRED htslib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${HTSLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${HTSLIB_LIBRARIES})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

