idf_component_register(SRCS "NMEABridge.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES Interface NMEA NMEALineSource StatsManager StatCounter DataModel
                                Logger Error)
