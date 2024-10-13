#ifndef EDID_MANAGER_H
#define EDID_MANAGER_H



#include <eeprom/at24cxx.h>
#include <eeprom/edid_struct.h>
#include <eeprom/metadata_struct.h>


#define MONITOR_ADDRESS 0x50 // 7 Bit Address of the Monitor
// Memory Map of the EEPROM
// Defined by EEPROM Model
#define EDID_START_ADDR     0x0000          // Start Address of the EDID Slots
#define EDID_SLOT_COUNT     15              // 15 EDID Slots
#define EEPROM_SIZE         4096            // 4KB EEPROM Size

// Defined by the EDID Standard and own memory layout
#define NORMAL_EDID_SIZE    128             // 128 Bytes for the Normal EDID
#define EDID_EXTENSION_SIZE 128             // 128 Bytes for the Extended EDID
#define EDID_SLOT_SIZE      256             // 256 Bytes for each EDID Slot
#define EDID_META_DATA_ADDR EDID_SLOT_SIZE * EDID_SLOT_COUNT - 1  // (3839) Start Address of the Meta Data
#define EDID_META_DATA_SIZE 16              // 16 Bytes for the Meta Data per Slot
#define EXTRA_DATA_ADDR     EDID_META_DATA_ADDR + EDID_META_DATA_SIZE * EDID_SLOT_COUNT  // (4079) Start Address of the Extra Data



#define EEPROM_ADDR_DEFAULT 0x50
#define EEPROM_ADDR_EXTRA 0x51


#define EEPROM_CLEAR_PASSWORD 0x11335577


namespace EEPROM{typedef enum{
    SUCCESS,
    SLOT_OUT_OF_RANGE,
    SAME_SLOT,
    SLOT_WRITE_PROTECT,
    MONITOR_NO_ANSWER,
    EEPROM_CLEAR_PROTECTION,
    COMMUNICATION_ERROR
    } State;

}







class EDIDManager {

private:
    Slot ramSlot;   // RAM Slot for the current EDID Slot
    Metadata ramMetaData;  // RAM Meta Data for the current EDID Slot

    GPIO_TypeDef* addrCtlPort;
    uint16_t addrCtlPin;

    GPIO_TypeDef* wpPort;
    uint16_t wpPin;

    AT24CXX eeprom;
    uint32_t clearProtectionPassword;

    void setDefaultAddress();
    void setExtraAddress();

    // GPIO: 1: Enabled; 0: Disabled
    void writeProtect(bool enable);

    void cleanRamSlot();

    void cleanRamMetaData();

    // Read the Slot and Meta Data from the EEPROM to the RAM (No Write Protect Check, Address change)
    void slotToRAM(uint8_t slot);

    // Write the Slot and Meta Data from the RAM to the EEPROM (No Write Protect Check, Address change)
    void ramToSlot(uint8_t slot);

    // Read the EDID from the Monitor to the RAM
    EEPROM::State readMonitorEDID();

    // True: Write Protect is applied; False: Write Protect is disabled
    EEPROM::State getSlotMetaDataFlags(uint8_t slot, u8* flags);

    // Enable/Disable Write Protect for a Slot
    bool setSlotWriteProtect(uint8_t slot, bool enable);


public:

    EDIDManager(GPIO_TypeDef* addrCtlPort, uint16_t addrCtlPin, GPIO_TypeDef* wpPort, uint16_t wpPin);
    
    void init();

    EEPROM::State cleanSlot(uint8_t slot, bool deepClean = false);

    void disableEEPROMClearProtection();

    EEPROM::State cleanEEPROM();

    // Load the EDID and MetaData from the EEPROM to the RAM
    EEPROM::State loadSlot(uint8_t slot);

    // Save the EDID and MetaData from the RAM to the EEPROM
    EEPROM::State saveSlot(uint8_t slot);


    u8* getEDIDBuffer();


    EEPROM::State moveSlot(uint8_t slot, uint8_t targetSlot);

    // Copys the EDID Data from the Slot x to Slot 0
    EEPROM::State applySlot(uint8_t slot);

    // Copys the EDID Data from the Monitor to slot
    EEPROM::State cloneToSlot(uint8_t slot);

    

};





#endif // EDID_MANAGER_H