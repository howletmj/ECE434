This folder is for any files I need for my RFID reader project.
The sub folder MFRCBellDisplay contains the makefile, setup.sh and any additional scripts that need to be run in order to get the project working.
The program relies on MFRC522.c. Any additional files in the FinalProject directory are failed attempts at getting the MFRC and TFT9225 arduino libraries converted to BeagleBone libraries. The MFRC522.c file was not made by me, link below.  I had to make modifications to the structs because the gcc compiler would compile but not biuld the structs correctly and failed to write to the spi bus.
original source for MFRC522.c :https://github.com/PeakUp/BeagleBoneBlack-MFRC522_Library
