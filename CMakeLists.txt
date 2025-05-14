cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(projectName General_pwn_tool_lol_oWo)
set(assetDir assets)
set(sourceDir ${assetDir}/source)
set(libDir ${assetDir}/lib)

project(${projectName})
add_executable(${projectName} ${sourceDir}/main.cpp)

add_subdirectory(${libDir}/core)
add_subdirectory(${libDir}/ui)
add_subdirectory(${libDir}/dataStructures)

target_link_libraries(${projectName} PRIVATE 
    core
    ui
    dataStructures
)
