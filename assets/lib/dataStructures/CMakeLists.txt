
add_library(dataStructures STATIC
    src/sessionData.cpp
    src/pythonProcess.cpp
)

target_include_directories(dataStructures
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(dataStructures
    PRIVATE core
    PRIVATE ui
)