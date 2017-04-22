cmake_minimum_required(VERSION 3.7)
project(soco2root)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        src/FSUtils.cpp
        src/Hit.cpp
        src/Event.cpp
        src/EventReader.cpp
        src/RootWriter.cpp
        src/Soco2Root.cpp
        )

find_package(ROOT REQUIRED)
include_directories(src ${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})
#add_definitions(${ROOT_CXX_FLAGS})
add_executable(soco2root ${SOURCE_FILES})
target_link_libraries(soco2root ${ROOT_LIBRARIES})
