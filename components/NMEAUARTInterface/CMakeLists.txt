idf_component_register(SRCS "NMEAUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES UARTInterface Interface NMEA Error Logger driver)
