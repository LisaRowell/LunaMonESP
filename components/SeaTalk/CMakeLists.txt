idf_component_register(SRCS "SeaTalkInterface.cpp"
                            "SeaTalkParser.cpp"
                            "SeaTalkMaster.cpp"
                            "SeaTalkCommand.cpp"
                            "SeaTalkLine.cpp"
                            "SeaTalkLampIntensity.cpp"
                            "SeaTalkWriteTester.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES Interface InstrumentData SeaTalkNMEABridge StatsManager StatCounter
                                DataModel TaskObject FixedPoint Logger)
