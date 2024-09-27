idf_component_register(SRCS "RMTUART.cpp"
                            "RMTUARTReceiver.cpp"
                            "RMTSymbolReader.cpp"
                            "RMTCharBuilder.cpp"
                            "RMTUARTTransmitter.cpp"
                            "RMTUARTBitStreamer.cpp"
                            "RMTSymbolWriter.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES Interface TaskObject Logger Error driver esp_driver_rmt)
