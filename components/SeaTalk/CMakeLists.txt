idf_component_register(SRCS "SeaTalkParser.cpp"
                            "SeaTalkCommand.cpp"
                            "SeaTalkLine.cpp"
                            "SeaTalkLampIntensity.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES InstrumentData StatsManager StatCounter DataModel FixedPoint Logger)
