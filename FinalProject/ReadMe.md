# About this Project
This project is for ece 434 embedded linux course for Rose-Hulman Institute of Technology. My project and more about it is located here:[https://elinux.org/ECE497_Project_-_RFID_Scanner](https://elinux.org/ECE497_Project_-_RFID_Scanner)

# Installation
1. Clone repo
2. Run setup.sh in MFRCBellDisplay subfolder
3. Make clean in MFRCBellDisplay subfolder
4. Make in MFRCBellDisplay subfolder
5. Modify logtosheetd.py to use your own google sheet
5. Run MFRC522

## Additional Information
This folder is for any files I need for my RFID reader project.
The sub folder MFRCBellDisplay contains the makefile, setup.sh and any additional scripts that need to be run in order to get the project working.
The MFRC522.c file was not made by me, link below.  I had to make modifications to the structs because the gcc compiler would compile but not build the structs correctly and failed to write to the spi bus.
original source for MFRC522.c :https://github.com/PeakUp/BeagleBoneBlack-MFRC522_Library
