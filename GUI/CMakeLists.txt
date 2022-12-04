cmake_minimum_required(VERSION 3.18)

set(CMAKE_AUTOMOC ON)

project(DMB_GUI VERSION 0.1)

file(GLOB QML qml/*.qml)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_add_executable(${PROJECT_NAME}

)

qt_add_qml_module(${PROJECT_NAME}
    URI ${PROJECT_NAME}
    QML_FILES ${QML}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Quick
)