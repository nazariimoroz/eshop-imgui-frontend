project(eshop_logic)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC
    include/logic/user.h
    src/user.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        ./include/ )
