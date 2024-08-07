idf_component_register(SRCS "NMEAWiFiInterface.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES WiFiInterface TaskObject NMEA Logger)
