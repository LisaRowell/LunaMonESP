idf_component_register(SRCS "Interface.cpp"
                            "InterfaceParams.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES StatCounter StatsManager DataModel TaskObject Logger Error)
