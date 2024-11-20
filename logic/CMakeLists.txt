project(eshop_logic)

set(CMAKE_CXX_STANDARD 20)

find_package(reflect-cpp REQUIRED)

add_library(${PROJECT_NAME} STATIC
        include/logic/models/user_model.h
        src/user_model.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        ./include/ )

target_link_libraries(${PROJECT_NAME} PRIVATE
        reflect-cpp::reflect-cpp)
