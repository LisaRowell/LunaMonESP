idf_component_register(SRCS "UARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES Interface Error Logger driver)
