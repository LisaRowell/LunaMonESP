idf_component_register(SRCS "STALK.cpp"
                            "STALKSource.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEALineSource DataModel StatCounter StatsManager Logger Error)
