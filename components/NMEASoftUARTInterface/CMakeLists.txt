idf_component_register(SRCS "NMEASoftUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES SoftUARTInterface Interface NMEA Error Logger driver)
