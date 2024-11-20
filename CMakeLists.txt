cmake_minimum_required(VERSION 3.29)
project(eshop_cpp_frontend)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(logic)
add_subdirectory(gui)

add_executable(${PROJECT_NAME}
        main.cpp
        .env.h)

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d9
        d3dcompiler
        dxgi

        eshop_logic
        eshop_gui)
