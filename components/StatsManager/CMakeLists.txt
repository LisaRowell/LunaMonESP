idf_component_register(SRCS "StatsManager.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES TaskObject PassiveTimer Logger esp_timer)
