CMAKE_MINIMUM_REQUIRED(VERSION 3.18)
SET(CMAKE_CXX_STANDARD 17)

PROJECT(BilibiliDMB)

SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)

SET(FORMS
    src/bilibiliDMB.ui
)

SET(HEADERS
    src/bilibiliDMB.h
    src/utils.h
)

SET(SOURCES
    src/main.cpp
    src/bilibiliDMB.cpp
)

ADD_EXECUTABLE(${PROJECT_NAME}
    ${FORMS}
    ${HEADERS}
    ${SOURCES}
)

FIND_PACKAGE(Qt6 COMPONENTS Core Gui Widgets WebSockets REQUIRED)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebSockets
)
