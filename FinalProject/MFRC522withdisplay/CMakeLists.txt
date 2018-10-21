cmake_minimum_required (VERSION 2.8)
project (FinalProject)
add_executable (DisplayMFRC522 MFRC522.cpp gpio-utils.c TFT_ILI9225_SPI.cpp)
