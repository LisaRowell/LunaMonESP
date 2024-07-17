idf_component_register(SRCS "main.cpp" "LunaMon.cpp"
                       INCLUDE_DIRS "."  "../etl/include"
                       REQUIRES NMEADataModelBridge NMEA DataModel AIS MQTT I2CMaster WiFiManager
                                NMEAWiFiSource EnvironmentalMon StatusLED LogManager StatsManager
                                Logger Error driver nvs_flash esp_timer)
