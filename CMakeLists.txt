cmake_minimum_required(VERSION 3.27)
project(Laboratorio4 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall")

add_executable(
    ${PROJECT_NAME}
    guerrero_w_shm_sem.c
    constantes.h
)

target_link_libraries(${PROJECT_NAME} m)