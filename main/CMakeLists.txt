idf_component_register(SRCS "main.cpp" "LunaMon.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES I2CMaster EnvironmentalMon StatusLED Logger)
