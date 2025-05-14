add_library(ui STATIC
    src/ui.cpp
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
