idf_component_register(SRCS "STALK.cpp"
                            "STALKSource.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES SeaTalk NMEALineSource DataModel StatCounter StatsManager Logger
                                Error)
