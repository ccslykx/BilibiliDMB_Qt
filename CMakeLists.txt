cmake_minimum_required(VERSION 3.18)
project(BilibiliDMB)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE "src/*.h" "src/*.cpp" "src/*.ui")
add_executable(${PROJECT_NAME} ${SOURCE})

find_package(Qt6 COMPONENTS Core Gui Widgets WebSockets REQUIRED)

target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
)
