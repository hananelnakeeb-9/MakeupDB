cmake_minimum_required(VERSION 3.19)
project(MakeupDB LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(MakeupDB
    main.cpp
)

target_link_libraries(MakeupDB
    PRIVATE
        Qt::Core
)

install(TARGETS MakeupDB
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
