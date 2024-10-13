# EDID Emulator


EDID Emulator is a Open Source Adapter for Reading, Cloning and Editing EDIDs.

The PCB ist hosted on OSHWLab and can be viewd with EasyEda:
Project-Link: https://oshwlab.com/mika.schaechinger/edid-emulator

The Adapter is connected between a HDMI/DVI source and a Display replaces the EDID and Hotplug Detect of the Display.

Use cases:

Keeping monitors connected to a PC while switching between different PCs with an inexpensive HDMI switch.
Controlling the display settings when using an HDMI splitter.
(Hopefully) use the HDMI Port of the TAng Nano 9k Dev board as a Input
Create and save multiple EDIDs over Serial Interface
Clone EDIDs of other Displays
Inspect and read out EDIDs over Serial Interface
 
It has a USB-C Connector to access the Serial Interface, and two HDMI Ports for Input and Output.

The PCB is a 6-Layer PCB, so the TMDS lines have enough shielding.


## Microcontroller:

To control the Adapter, a ch32v003 from WCH is used. It has a UART Interface over USB-C, I2C for the EEPROM and GPIOs for the Button and LEDs.
It can controll the EEPROM I2C Adress and can detect the 5V Power from the HDMI Source. 


## Serial Interface:

The Serial Interface over USB-C is used to read out, write and edit EDIDs. The Serial Settings are:
9600 Baud
8 Data Bits
1 Stop Bit
No Parity
No Flow Control

The Commands are:

*Command*                       - *Description*

version                         - Shows the Version of the Firmware
help                            - Shows this help
man <command>                   - Shows the manual for the given command 

list                            - Lists all EDIDs status in the EEPROM

read                            - Reads the EDID from the Slot 0 in hex
read <EDID-SLOT-Index>          - Reads the EDID from given Slot in hex
read <EDID-SLOT-Index> -hex     - Reads the EDID from given Slot in hex
read <EDID-SLOT-Index> -json    - Reads the EDID from given Slot in json format with checksum
read -all                       - Reads all EDIDs as json with checksum

write <EDID-SLOT-Index> <EDID-Data> -f - Writes the EDID to the given Slot


## Button Interface:

The Adapter has a Button to control the Adapter and two LEDs to give feedback...


## EDID Storrage:

The Adapter has a 32kBit EEPROM. With 256 Bytes per (extended) EDID, this allows for 1 active EDID + 14 stored EDIDs + Metadata to be stored.

Metadata 16byte per EDID:
- 0 Byte:
    - Bit 0: EDID is valid
    - Bit 1: write protected
    - Bit 2: Clone Status (0: Original (Cloned), 1: Edited)
    - Bit 3: EDID is extended
    - Bit 4-7: Reserved
- 1-15 Byte: Display Name (UTF-8)

Last 16 Byte Block:
- 0 Byte: Active EDID Slot Index
- 1-15 Byte: Reserved
