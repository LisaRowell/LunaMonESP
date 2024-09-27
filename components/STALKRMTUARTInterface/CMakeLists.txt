idf_component_register(SRCS "STALKRMTUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEALineSource RMTUARTInterface Interface STALK SeaTalk PassiveTimer
                                Error Logger driver)
