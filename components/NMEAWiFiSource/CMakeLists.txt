idf_component_register(SRCS "NMEAWiFiSource.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES TaskObject WiFiManager NMEA Logger)
