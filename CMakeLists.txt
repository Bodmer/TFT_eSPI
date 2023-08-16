
idf_component_register(SRCS "TFT_eSPI.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES "arduino")

function(maybe_add_component component_name)
    idf_build_get_property(components BUILD_COMPONENTS)
    message(STATUS "Components: ${components}")
    if (${component_name} IN_LIST components)
        idf_component_get_property(lib_name ${component_name} COMPONENT_LIB)
        target_link_libraries(${COMPONENT_LIB} PUBLIC ${lib_name})
    endif()
endfunction()

maybe_add_component(esp_lcd)