idf_component_register(SRCS "DataModelBridge.cpp"
                            "AutoPilotBridge.cpp"
                            "GPSBridge.cpp"
                            "WaterBridge.cpp"
                            "WindBridge.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEA InstrumentData DataModel)
