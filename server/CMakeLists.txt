cmake_minimum_required(VERSION 3.6.3)
project(aw_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp physics.h physics.cpp logic.cpp logic.hpp server.cpp server.hpp)
add_executable(aw_server ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "../cmake_modules")
find_package(SFML REQUIRED system window graphics network)
find_package(Box2D REQUIRED)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(aw_server ${SFML_LIBRARIES})
endif()

#set headers stuff
set(INCLUDE_DIRS ${BOX2D_INCLUDE_DIR})

#set lib stuff
set(DEPEND_LIBS ${BOX2D_LIBRARY})

include_directories(${INCLUDE_DIRS})
target_link_libraries(aw_server ${DEPEND_LIBS})