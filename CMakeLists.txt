cmake_minimum_required(VERSION 3.27)
project(juego-rol C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(EXEC_FILENAME ${PROJECT_NAME}.bin)

add_executable(
    ${EXEC_FILENAME}
    guerrero_w_shm_sem.c
    constantes.h
)

target_link_libraries(${EXEC_FILENAME} m)
