idf_component_register(SRCS "NMEARMTUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES RMTUARTInterface Interface NMEA Error Logger driver)
