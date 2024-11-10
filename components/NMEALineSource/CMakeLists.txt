idf_component_register(SRCS "NMEALine.cpp"
                            "NMEALineSource.cpp"
                            "NMEALineWalker.cpp"
                            "NMEATalker.cpp"
                            "NMEAMsgType.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES StatsManager StatCounter DataModel CharacterTools StringTools Logger
                                Error)
