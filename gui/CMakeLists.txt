cmake_minimum_required(VERSION 3.29)
project(eshop_gui)

set(CMAKE_CXX_STANDARD 20)

find_package(reflect-cpp REQUIRED)

file(GLOB IMGUI_SOURCES
        ../imgui/*
        ../imgui/backend/imgui_impl_dx9.h
        ../imgui/backend/imgui_impl_dx9.cpp
        ../imgui/backend/imgui_impl_win32.h
        ../imgui/backend/imgui_impl_win32.cpp)

add_library(${PROJECT_NAME} STATIC
        ../.env.h
        ${IMGUI_SOURCES}

        include/gui/gui.h
        src/gui.cpp
        include/gui/base_window_t.h
        src/base_window_t.cpp

        include/gui/windows/shop_list_window_t.h
        src/shop_list_window_t.cpp
        include/gui/windows/user_profile_window_t.h
        src/user_profile_window_t.cpp
        src/auth_window.cpp
        include/gui/windows/auth_window.h
        src/imgui_ex.cpp
        include/gui/imgui_ex.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        include
        ../imgui)

target_link_libraries(${PROJECT_NAME} PUBLIC
        d3d9
        d3dcompiler
        dxgi

        eshop_logic
        reflect-cpp::reflect-cpp)
