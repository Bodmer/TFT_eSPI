
idf_component_register(SRCS "TFT_eSPI.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES arduino)

add_definitions(-DTFT_eSPI_COMPONENT)