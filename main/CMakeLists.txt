idf_component_register(SRCS "main.cpp" "LunaMon.cpp"
                       INCLUDE_DIRS "."  "../etl/include"
                       REQUIRES NMEADataModelBridge NMEA NMEAWiFiSource STALK NMEAUARTInterface
                                UARTInterface Interface DataModel AIS MQTT I2CMaster WiFiManager
                                EnvironmentalMon StatusLED LogManager StatsManager Logger Error
                                driver nvs_flash esp_timer)
