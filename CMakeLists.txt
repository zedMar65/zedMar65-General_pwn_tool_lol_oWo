cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

<<<<<<< HEAD
set(projectName General_pwn_tool_lol_oWo)
set(assetDir assets)
set(sourceDir assetDir/source)
set(libDir assetDir/lib)

project(projectName)
add_executable(projectName sourceDir/main.cpp)

add_subdirectory(libDir)

target_link_libraries()
=======
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
>>>>>>> e34ea514060ee0202d76fdf2d088a231f8762f36
