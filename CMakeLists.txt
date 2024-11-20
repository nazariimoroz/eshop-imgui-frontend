cmake_minimum_required(VERSION 3.29)
project(eshop_cpp_frontend)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(logic)

file(GLOB IMGUI_SOURCES
        ./imgui/*
        imgui/backend/imgui_impl_dx9.h
        imgui/backend/imgui_impl_dx9.cpp
        imgui/backend/imgui_impl_win32.h
        imgui/backend/imgui_impl_win32.cpp)

add_executable(${PROJECT_NAME}
        main.cpp
        .env.h
        ${IMGUI_SOURCES}

        gui/gui.cpp
        gui/gui.h
        gui/base_window.cpp
        gui/base_window.h
        gui/shop_list_window.cpp
        gui/shop_list_window.h
        gui/user_profile_window.h
        gui/user_profile_window.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        .
        imgui)

target_link_libraries(${PROJECT_NAME} PRIVATE
        d3d9
        d3dcompiler
        dxgi

        eshop_logic)
