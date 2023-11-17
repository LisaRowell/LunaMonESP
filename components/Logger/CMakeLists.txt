idf_component_register(SRCS "Logger.cpp" "ESPError.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES Error esp_netif)
