idf_component_register(SRCS "main.cpp" "LunaMon.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES DataModel MQTT I2CMaster WiFiManager NMEAWiFiSource EnvironmentalMon
                                StatusLED Logger Error driver nvs_flash)
