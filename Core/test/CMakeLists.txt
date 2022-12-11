cmake_minimum_required(VERSION 3.18)

set(CMAKE_AUTOMOC ON)

project(Bilibili_test VERSION 0.1)

file(GLOB HEADER CMAKE_CONFIGURE_DEPENDS ../include/Core/*.h)
file(GLOB SOURCE CMAKE_CONFIGURE_DEPENDS ../src/*.cpp)

add_executable(${PROJECT_NAME} 
    ${HEADER}
    ${SOURCE} 
    main.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core WebSockets)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::WebSockets
)