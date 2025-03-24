#include "edid/EdidManager.h"


EdidManager::EdidManager(EEPROMAccessManager* eeprom) : eeprom(eeprom) {
}


EdidManager::Result EdidManager::EEPROMToRam(uint8_t slotIndex, uint8_t ramIndex) {
    if (slotIndex >= kMaxSlots) {
        return Result::InvalidSlotIndex;
    }
    if (ramIndex >= kRamSlotCount) {
        return Result::InvalidRamIndex;
    }
    
    // Read slot from EEPROM
    uint16_t slotAddress = getSlotAddress(slotIndex);
    Slot& slot = ramSlots[ramIndex];

    AT24CXX::Result result = eeprom->readFromEEPROM(slotAddress, slot.data, sizeof(Slot));
    if (result != AT24CXX::Result::Success) {
        return Result::EEPROMError;
    }

    
    // Read metadata from EEPROM
    uint16_t metadataAddress = getMetadataAddress(slotIndex);
    Metadata& metadata = ramMetadata[ramIndex];

    result = eeprom->readFromEEPROM(metadataAddress, metadata.data, sizeof(Metadata));
    if (result != AT24CXX::Result::Success) {
        return Result::EEPROMError;
    }

    return Result::Success;
}


EdidManager::Result EdidManager::RamToEEPROM(uint8_t slotIndex, uint8_t ramIndex) {
    if (slotIndex >= kMaxSlots) {
        return Result::InvalidSlotIndex;
    }
    if (ramIndex >= kRamSlotCount) {
        return Result::InvalidRamIndex;
    }

    Slot& slot = ramSlots[ramIndex];
    Metadata& meta = ramMetadata[ramIndex];

    // Write EDID-Daten
    AT24CXX::Result result = eeprom->writeToEEPROM(getSlotAddress(slotIndex), slot.data, sizeof(Slot));
    if (result != AT24CXX::Result::Success) {
        return Result::EEPROMError;
    }

    // Write Metadaten
    result = eeprom->writeToEEPROM(getMetadataAddress(slotIndex), meta.data, sizeof(Metadata));
    if (result != AT24CXX::Result::Success) {
        return Result::EEPROMError;
    }

    return Result::Success;
}


EdidManager::Result EdidManager::MonitorToRam(uint8_t ramIndex) {
    if (ramIndex >= kRamSlotCount) {
        return Result::InvalidRamIndex;
    }

    Slot& slot = ramSlots[ramIndex];
    Metadata& meta = ramMetadata[ramIndex];

    // Step 1: Read fisrst standard EDID block from monitor
    AT24CXX::Result result = eeprom->readFromMonitor(0x00, slot.data, 128);
    if (result != AT24CXX::Result::Success) {
        return Result::EEPROMError;
    }

    // Step 2: Check if EDID is extended
    bool hasExtension = (slot.edid.extensionFlag > 0);

    if (hasExtension) {
        // Stepp 3: Read extension block from monitor
        result = eeprom->readFromMonitor(0x80, slot.extensionBlock, 128);
        if (result != AT24CXX::Result::Success) {
            return Result::EEPROMError;
        }
    } else {
        // No extension: Fill extension block with 0xFF
        for (uint8_t i = 0; i < 128; ++i)
            slot.extensionBlock[i] = 0xFF;
    }

    // Metadaten initialisieren
    meta.flags = 0;
    meta.edid_valid = 1;
    meta.clone_status = 0;
    meta.edid_extended = hasExtension;

    // Name auf "Unnamed" setzen
    const char* name = "Unnamed";
    for (uint8_t i = 0; i < sizeof(meta.edidName); ++i) {
        meta.edidName[i] = name[i] ? name[i] : '\0';
    }

    return Result::Success;
}


uint16_t EdidManager::getSlotAddress(uint8_t slotIndex) const {
    return slotIndex * sizeof(Slot);
}


uint16_t EdidManager::getMetadataAddress(uint8_t slotIndex) const {
    constexpr uint16_t metadataBlockBase = AT24CXX_SIZE_BYTES - sizeof(Slot);
    return metadataBlockBase + slotIndex * sizeof(Metadata);
}
