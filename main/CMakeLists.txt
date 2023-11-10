idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES I2CMaster BME280Driver FixedPoint Logger Error)
