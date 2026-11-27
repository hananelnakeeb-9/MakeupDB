
cmake_minimum_required(VERSION 3.19)

project(MakeupDB LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

# ==========================================
# MAIN APPLICATION
# ==========================================

qt_add_executable(MakeupDB
    main.cpp
    Product.h
    Database.h
    Database.cpp
    Index.h
    HashIndex.h
    HashIndex.cpp
    AVLIndex.h
    AVLIndex.cpp
)

target_include_directories(MakeupDB
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MakeupDB
    PRIVATE
        Qt::Core
)

# ==========================================
# HASH TABLE TESTS
# ==========================================

add_executable(HashIndexTest
    HashIndexTest.cpp
    Index.h
    HashIndex.h
    HashIndex.cpp
)

target_include_directories(HashIndexTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==========================================
# AVL TREE TESTS
# ==========================================

add_executable(AVLIndexTest
    AVLIndexTest.cpp
    Index.h
    AVLIndex.h
    AVLIndex.cpp
)

target_include_directories(AVLIndexTest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# ==========================================
# INSTALLATION
# ==========================================

install(TARGETS MakeupDB
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
