idf_component_register(SRCS "SoftUARTInterface.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES Interface SoftwareUART Logger Error esp_driver_gpio driver)
