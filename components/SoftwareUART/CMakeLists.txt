idf_component_register(SRCS "SoftwareUART.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES TaskObject Logger Error esp_driver_gpio driver esp_timer)
