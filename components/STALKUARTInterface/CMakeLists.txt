idf_component_register(SRCS "STALKUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES NMEALineSource UARTInterface Interface STALK Error Logger driver)
