cmake_minimum_required(VERSION 3.24)

project(texteditor)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

set(ALL_SOURCES src/main.cpp src/Observe.cpp src/Observe.h src/Adapter.cpp src/Adapter.h)

#for controller sources
set(CONTROLLER_HEADERS
        src/controller/Controller.h)
set(CONTROLLER_SOURCES
        src/controller/Controller.cpp
        ${CONTROLLER_HEADERS})
list(APPEND ALL_SOURCES ${CONTROLLER_SOURCES})
source_group("Controller" FILES ${CONTROLLER_SOURCES})


#for model sources
set(MODEL_HEADERS
        src/model/Model.h)
set(MODEL_SOURCES
        src/model/Model.cpp
        ${MODEL_HEADERS})
list(APPEND ALL_SOURCES ${MODEL_SOURCES})
source_group("Model" FILES ${MODEL_SOURCES})



#for view sources
set(VIEW_HEADERS
        src/view/View.h)
set(VIEW_SOURCES
        src/view/View.cpp
        ${VIEW_HEADERS})
list(APPEND ALL_SOURCES ${VIEW_SOURCES})
source_group("View" FILES ${VIEW_SOURCES})



add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_link_libraries(${PROJECT_NAME} mystring ncurses clip)
