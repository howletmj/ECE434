SHELL := /bin/sh
CC    := gcc
CFLAGS := -Wall
TARGET1 := spidevtest
TARGET2 := MFRC522
all: $(TARGET1) $(TARGET2)
	
$(TARGET1): ../$(TARGET1).c
	$(CC) $(CFLAGS) ../$(TARGET1).c -o $(TARGET1)
$(TARGET2): ../$(TARGET2).c
	$(CC) $(CFLAGS) ../$(TARGET2).c -o $(TARGET2)
clean:
	-rm $(TARGET1)
	-rm $(TARGET2)
