project(eshop_logic)

set(CMAKE_CXX_STANDARD 20)

find_package(yyjson REQUIRED)
find_package(reflect-cpp REQUIRED)
find_package(httplib REQUIRED)
find_package(bshoshany-thread-pool REQUIRED)
find_package(stduuid REQUIRED)

add_library(${PROJECT_NAME} STATIC
        include/logic/models/user_model.h
        src/user_model.cpp
        src/cache.cpp
        include/logic/cache.h
        src/user_fabric.cpp
        include/logic/fabrics/user_fabric.h
        src/task_manager.cpp
        include/logic/utilities/task_manager.h
        src/category_model.cpp
        include/logic/models/category_model.h
        src/product_model.cpp
        include/logic/models/product_model.h
        src/product_fabric.cpp
        include/logic/fabrics/product_fabric.h
        src/base_payment.cpp
        include/logic/payment/base_payment.h
        src/google_payment.cpp
        include/logic/payment/google_payment.h)

target_include_directories(${PROJECT_NAME} PUBLIC
        ./include/ )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        yyjson::yyjson
        reflect-cpp::reflect-cpp
        bshoshany-thread-pool::bshoshany-thread-pool
        PRIVATE
        httplib::httplib
        stduuid::stduuid)
