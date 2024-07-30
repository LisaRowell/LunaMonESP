idf_component_register(SRCS "NMEAUARTSource.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES TaskObject NMEA AIS DataModel Error Logger driver)
