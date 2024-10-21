idf_component_register(SRCS "NMEAServer.cpp"
                            "NMEAClient.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEALineSource StatsManager DataModel TaskObject WiFiManager Logger
                                Error)
