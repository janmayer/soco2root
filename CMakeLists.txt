cmake_minimum_required(VERSION 3.7)
project(soco2root)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

set(SOURCE_FILES
        main.cpp
        src/FSUtils.cpp
        src/Hit.cpp
        src/Event.cpp
        src/EventReader.cpp
        src/Soco2Root.cpp
        )

find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

include_directories(src ${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIR})

# Dictionary and shared shared library for SOCO::Event and SOCO::Hit
root_generate_dictionary(G__SOCO src/Hit.h src/Event.h LINKDEF SOCOLinkDef.h)
add_library(SOCO SHARED src/Hit.cpp src/Event.cpp G__SOCO.cxx)
target_link_libraries(SOCO ${ROOT_LIBRARIES})

add_executable(soco2root ${SOURCE_FILES} G__SOCO.cxx)
target_link_libraries(soco2root ${ROOT_LIBRARIES})
