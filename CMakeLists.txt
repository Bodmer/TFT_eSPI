if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # Top-level CMake project for PC simulator
    cmake_minimum_required(VERSION 3.10)
    project(TFT_eSPI_Simulator)

    set(CMAKE_CXX_STANDARD 11)

    add_compile_options(-Wno-non-c-typedef-for-linkage)
    add_compile_options(-Wno-int-to-pointer-cast)

    include_directories(
            .
            Extensions
            Simulator
    )
    add_executable(simulator
            Simulator/main.cpp
            Simulator/render_sdl.cpp

            Simulator/Arduino.cpp
            TFT_eSPI.cpp
    )

    # SDL2
    find_library(SDL2_LIBRARY SDL2)
    if (SDL2_INCLUDE_DIR AND SDL2_LIBRARY)
        message(STATUS "Found SDL2: ${SDL2_LIBRARY}")
        target_include_directories(simulator PRIVATE ${SDL2_INCLUDE_DIR})
        target_link_libraries(simulator PRIVATE ${SDL2_LIBRARY})
    else ()
        message(FATAL_ERROR "SDL2 not found")
    endif ()
else ()
    # ESP-IDF component
    idf_component_register(SRCS "TFT_eSPI.cpp"
            INCLUDE_DIRS "."
            PRIV_REQUIRES arduino)
endif ()
