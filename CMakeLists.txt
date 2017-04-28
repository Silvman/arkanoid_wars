cmake_minimum_required(VERSION 3.6.3)
project(arkanoid_wars)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(arkanoid_wars ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")
find_package(SFML REQUIRED system window graphics)
find_package(Box2D REQUIRED)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(arkanoid_wars ${SFML_LIBRARIES})
endif()

#set headers stuff
set(INCLUDE_DIRS
        ${BOX2D_INCLUDE_DIR}
        )

#set lib stuff
set(DEPEND_LIBS
        ${BOX2D_LIBRARY}
        )

include_directories(${INCLUDE_DIRS})
target_link_libraries(arkanoid_wars ${DEPEND_LIBS})