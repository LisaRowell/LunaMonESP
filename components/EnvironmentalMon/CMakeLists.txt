idf_component_register(SRCS "EnvironmentalMon.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES DataModel I2CMaster StatusLED BME280Driver ENS160Driver FixedPoint
                                TaskObject Logger Error)
