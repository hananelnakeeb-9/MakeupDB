
cmake_minimum_required(VERSION 3.19)

project(MakeupDB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

# Main application
qt_add_executable(MakeupDB
    main.cpp
    Product.h
    Database.h
    Database.cpp
    Index.h
    HashIndex.h
    HashIndex.cpp
)

target_include_directories(MakeupDB PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MakeupDB
    PRIVATE
        Qt::Core
)

# Separate Hash Table test application
add_executable(HashIndexTest
    HashIndexTest.cpp
    HashIndex.h
    HashIndex.cpp
    Index.h
)

target_include_directories(HashIndexTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

install(TARGETS MakeupDB
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
