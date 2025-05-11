cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(TestProject)

add_executable(debug 
    src/main.cpp
)

add_subdirectory(lib)

# Only link what debug actually needs
target_link_libraries(debug PRIVATE 
    main
    utils
    data
    scripts
)