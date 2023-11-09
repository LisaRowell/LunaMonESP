idf_component_register(SRCS "main.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES I2CMaster BME280Driver Logger Error)
