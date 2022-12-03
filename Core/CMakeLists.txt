cmake_minimum_required(VERSION 3.18)

set(CMAKE_AUTOMOC ON)

project(DMB_Core VERSION 0.1)

file(GLOB SOURCE CMAKE_CONFIGURE_DEPENDS src/*.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_executable(${PROJECT_NAME} ${SOURCE})

find_package(Qt6 REQUIRED COMPONENTS Core WebSockets)

target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::WebSockets
)