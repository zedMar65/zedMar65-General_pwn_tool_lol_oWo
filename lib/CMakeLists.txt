add_library(main STATIC
    main.hpp
    globals.cpp
)

add_library(utils STATIC
    utils.cpp
    utils.hpp
)

add_library(data STATIC
    dataStructures.hpp
    data.hpp
    data.cpp
)

add_library(scripts STATIC
    PScripts.cpp
    PScriptUtils.cpp
    PScripts.hpp
)

# Include paths for headers (assumes they're all in the same dir)
target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(scripts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# utils needs globals from main
target_link_libraries(utils PUBLIC main)