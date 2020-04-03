cmake_minimum_required(VERSION 3.5)

if(APPLE)
    # Fix linking on 10.14+ See https://stackoverflow.com/questions/54068035
    link_directories(/usr/local/lib)
endif()

project(fastqcleaner LANGUAGES C)
set(SOURCES main.c auxilary.c bitset.c fqparser.c nthash.c)
set(HEADERS auxilary.h bitset.h fqparser.h nthash.h khash.h kroundup.h)
add_executable(fastqcleaner ${SOURCES} ${HEADERS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(ZLIB REQUIRED zlib)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ZLIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})

