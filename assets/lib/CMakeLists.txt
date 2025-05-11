add_library(pythonProcess STATIC
    pythonProcess.hpp
    pythonProcess_internal.hpp
    pythonProcess.cpp
)

target_include_directories(pythonProcess PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})