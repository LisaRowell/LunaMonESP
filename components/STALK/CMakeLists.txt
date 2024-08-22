idf_component_register(SRCS "STALKInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES SeaTalk Interface NMEALineSource DataModel StatCounter StatsManager
                                Logger Error)
