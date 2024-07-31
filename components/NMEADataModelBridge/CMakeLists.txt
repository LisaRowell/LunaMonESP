idf_component_register(SRCS "NMEADataModelBridge.cpp"
                            "NMEADepthBridge.cpp"
                            "NMEAGPSBridge.cpp"
                            "NMEAWaterBridge.cpp"
                            "NMEAWindBridge.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEA StatCounter StatsManager DataModel)
