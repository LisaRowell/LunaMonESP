idf_component_register(SRCS "MQTTBroker.cpp"
                            "MQTTConnection.cpp"
                            "MQTTConnectMessage.cpp"
                            "MQTTConnectAckMessage.cpp"
                            "MQTTMessage.cpp"
                            "MQTTString.cpp"
                            "MQTTUtil.cpp"
                       INCLUDE_DIRS "include" "../../etl/include"
                       REQUIRES TaskObject WiFiManager Logger Error)
