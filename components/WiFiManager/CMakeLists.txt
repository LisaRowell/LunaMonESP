idf_component_register(SRCS "WiFiManager.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES TaskObject Logger Error esp_netif esp_wifi)
