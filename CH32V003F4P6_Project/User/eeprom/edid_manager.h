#ifndef EDID_MANAGER_H
#define EDID_MANAGER_H



#include <eeprom/at24cxx.h>
#include <eeprom/edid_struct.h>



// Memory Map of the EEPROM
// Defined by EEPROM Model
#define EDID_START_ADDR     0x0000          // Start Address of the EDID Slots
#define EDID_SLOT_COUNT     15              // 15 EDID Slots
#define EEPROM_SIZE         4096            // 4KB EEPROM Size

// Defined by the EDID Standard and own memory layout
#define EDID_SLOT_SIZE      256             // 256 Bytes for each EDID Slot
#define EDID_META_DATA_ADDR EDID_SLOT_SIZE * EDID_SLOT_COUNT - 1  // (3839) Start Address of the Meta Data
#define EDID_META_DATA_SIZE 16              // 16 Bytes for the Meta Data per Slot
#define EXTRA_DATA_ADDR     EDID_META_DATA_ADDR + EDID_META_DATA_SIZE * EDID_SLOT_COUNT  // (4079) Start Address of the Extra Data



#define EEPROM_ADDR_DEFAULT 0x50
#define EEPROM_ADDR_EXTRA 0x51



class EDIDManager {

private:

    Slot ramSlot;   // RAM Slot for the current EDID Slot
    uint8_t metaDataBuffer[16];

    GPIO_TypeDef* addrCtlPort;
    uint16_t addrCtlPin;

    GPIO_TypeDef* wpPort;
    uint16_t wpPin;

    AT24CXX eeprom;

    void setDefaultAddress();
    void setExtraAddress();

public:

    EDIDManager(GPIO_TypeDef* addrCtlPort, uint16_t addrCtlPin, GPIO_TypeDef* wpPort, uint16_t wpPin);
    
    void init();


};





#endif // EDID_MANAGER_H