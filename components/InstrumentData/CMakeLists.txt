idf_component_register(SRCS "InstrumentData.cpp"
                            "AutoPilotData.cpp"
                            "GPSData.cpp"
                            "WaterData.cpp"
                            "WindData.cpp"
                            "InstrumentGroup.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES StatCounter StatsManager DataModel Logger Error)
