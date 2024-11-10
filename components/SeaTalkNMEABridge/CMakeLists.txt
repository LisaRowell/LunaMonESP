idf_component_register(SRCS "SeaTalkNMEABridge.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEALineSource Interface StatCounter StatsManager Logger Error)
