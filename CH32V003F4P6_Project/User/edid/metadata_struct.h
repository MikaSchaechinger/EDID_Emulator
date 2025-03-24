#ifndef METADATA_STRUCT_H
#define METADATA_STRUCT_H


#include <ch32v00x.h>


#define EDID_VALID_MASK        0x01
#define WRITE_PROTECT_MASK     0x02
#define CLONE_STATUS_MASK      0x04
#define EDID_EXTENDED_MASK     0x08



typedef union{
    struct 
    {
        union{
            struct{
                u8 edid_valid       : 1;    // Bit 0: EDID is valid      
                u8 write_protect    : 1;    // Bit 1: Write Protect
                u8 clone_status     : 1;    // Bit 2: Clone Status (0: Original, 1: Edited)
                u8 edid_extended    : 1;    // Bit 3: EDID is extended
                u8 reserved         : 4;    // Bit 4-7: Reserved
            };
            u8 flags; 
        };

        uint8_t edidName[15];            // Name of the EDID Slot
    };
    uint8_t data[16];
} Metadata;





#endif // METADATA_STRUCT_H