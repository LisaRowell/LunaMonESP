idf_component_register(SRCS "NMEALine.cpp"
                            "NMEALineSource.cpp"
                            "NMEALineWalker.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES CharacterTools StringTools Logger Error)
