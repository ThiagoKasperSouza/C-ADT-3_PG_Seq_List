cmake_minimum_required(VERSION 3.10)
project(c_seq_list C)

find_package(PostgreSQL REQUIRED)

set(CMAKE_C_STANDARD 11)
include_directories(src/lib)

file(GLOB SOURCES "src/*c")

add_executable(program ${SOURCES})

# Inclui os cabecalhos (contem libpq-fe.h)
target_include_directories(program PRIVATE ${PostgreSQL_INCLUDE_DIRS})

# Vincula o arquivo de biblioteca da libpq
target_link_libraries(program PRIVATE ${PostgreSQL_LIBRARIES})

