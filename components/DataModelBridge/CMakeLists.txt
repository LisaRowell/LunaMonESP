idf_component_register(SRCS "DataModelBridge.cpp"
                            "AutoPilotBridge.cpp"
                            "DepthBridge.cpp"
                            "GPSBridge.cpp"
                            "WaterBridge.cpp"
                            "WindBridge.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEA StatCounter StatsManager DataModel)
