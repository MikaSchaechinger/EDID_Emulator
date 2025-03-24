#ifndef EDID_MANAGER_H
#define EDID_MANAGER_H


#include "eeprom/EEPROMAccessManager.h"
#include "edid/edid_struct.h"
#include "edid/metadata_struct.h"






class EdidManager{
public:
    enum class Result {
        Success,
        InvalidSlotIndex,
        InvalidRamIndex,
        InvalidMetadata,
        EEPROMError
    };

    EdidManager(EEPROMAccessManager* eeprom);

    static constexpr uint8_t kRamSlotCount = 2;
    static constexpr uint16_t kSlotSize = 256;   // 256 Bytes
    static constexpr uint8_t kMetadataSize = sizeof(Metadata);
    static constexpr uint8_t kMaxSlots = (AT24CXX_SIZE_BYTES / kSlotSize) - 1;  // -1 because of metadata


    Result EEPROMToRam(uint8_t slotIndex, uint8_t ramIndex);
    Result RamToEEPROM(uint8_t slotIndex, uint8_t ramIndex);
    Result MonitorToRam(uint8_t ramIndex);

    Slot& getRamSlot(uint8_t ramIndex);
    const Slot& getRamSlot(uint8_t ramIndex) const;

    Metadata& getRamMetadata(uint8_t ramIndex);
    const Metadata& getRamMetadata(uint8_t ramIndex) const;

    uint8_t getSlotCount() const { return kMaxSlots; }
    
private:
    EEPROMAccessManager* eeprom;

    Slot ramSlots[kRamSlotCount];
    Metadata ramMetadata[kRamSlotCount];

    uint16_t getSlotAddress(uint8_t slotIndex) const;
    uint16_t getMetadataAddress(uint8_t slotIndex) const;
};

#endif  // EDID_MANAGER_H