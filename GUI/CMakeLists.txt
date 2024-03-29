cmake_minimum_required(VERSION 3.18)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

project(DMB VERSION 0.1)

file(GLOB QML qml/*.qml)
file(GLOB QRC qml/*.qrc)
file(GLOB HEADER CMAKE_CONFIGURE_DEPENDS 
    include/GUI/*.h 
    ../Core/include/Core/*.h
)
file(GLOB SOURCE CMAKE_CONFIGURE_DEPENDS 
    ../Core/src/*.cpp
    src/*.cpp
)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core WebSockets Quick)

add_executable(app${PROJECT_NAME}
    ${HEADER}
    ${SOURCE}
    ${QRC}   # qrc文件需要添加到 add_executable 才能正常使用 https://doc.qt.io/qt-6/resources.html
)

qt_add_qml_module(app${PROJECT_NAME}
    URI ${PROJECT_NAME}
    VERSION 1.0
    QML_FILES ${QML}
)

target_include_directories(app${PROJECT_NAME} PUBLIC
    ../Core/include
    ../Core/include/Core
    include
)

target_link_libraries(app${PROJECT_NAME} PRIVATE 
    Qt6::Core
    Qt6::Quick 
    Qt6::WebSockets
)
