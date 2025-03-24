#ifndef EDID_STRUCT_H
#define EDID_STRUCT_H



#include <ch32v00x.h>


typedef struct{
    u8 t1;
    u8 t2;
    u8 mfg_rsvd;
} est_timings_t;


typedef struct{
    u8 hsize;
    u8 vfreq_aspect;
} std_timing_t;


typedef struct{
    u8 hactive_lo;
    u8 hblank_lo;
    u8 hactive_hblank_hi;
    u8 vactive_lo;
    u8 vblank_lo;
    u8 vactive_vblank_hi;
    u8 hsync_offset_lo;
    u8 hsync_pulse_width_lo;
    u8 vsync_offset_pulse_width_lo;
    u8 hsync_vsync_offset_pulse_width_hi;
    u8 width_mm_lo;
    u8 height_mm_lo;
    u8 width_height_mm_hi;
    u8 hborder;
    u8 vborder;
    u8 misc;
} detailed_pixel_timing_t;


typedef struct{
    u8 min_vfreq;
    u8 max_vfreq;
    u8 min_hfreq_khz;
    u8 max_hfreq_khz;
    u8 pixel_clock_mhz;
    u8 flags;
    union formula{
        // TODO: ???
    };
    u8 reserved;
    u8 hfreq_start_khz;
    u8 c;
    u16 m;  // __le16   ()  https://docs.huihoo.com/doxygen/linux/kernel/3.7/structdetailed__data__monitor__range.html
    u8 k;
    u8 j;
    u8 version;
    u8 data1;
    u8 data2;
    u8 supported_aspects;
    u8 supported_scalings;
    u8 preferred_refresh;
} detailed_data_monitor_range_t;


typedef struct{
    u8 white_yx_lo;
    u8 white_x_hi;
    u8 white_y_hi;
    u8 gamma;
} detailed_data_wpindex_t;


typedef struct{
    u8 code[3];
} cvt_timing_t;


typedef struct{
    u8 pad1;
    u8 type;
    u8 pad2;
    union data{
        u8 str[13];
        detailed_data_monitor_range_t range;
        detailed_data_wpindex_t color;
        std_timing_t timings[6];
        cvt_timing_t cvt[4];
    };
    
} detailed_non_pixel_t;


typedef struct{
    detailed_pixel_timing_t pixel_data;
    detailed_non_pixel_t other_data;
} detailed_timing_t;





typedef struct {
    u8 header[8];           // Header: Fixed 8-byte pattern (00 FF FF FF FF FF FF 00)
    u16 manufacturerID;      // Manufacturer ID
    u16 productCode;         // Product Code
    u32 serialNumber;        // Serial Number
    u8 weekOfManufacture;    // Week of Manufacture
    u8 yearOfManufacture;    // Year of Manufacture
    u8 edidVersion;          // EDID Version
    u8 edidRevision;         // EDID Revision
    u8 videoInputType;       // Video Input Definition
    u8 horizontalSize;       // Horizontal Screen Size in cm
    u8 verticalSize;         // Vertical Screen Size in cm
    u8 displayGamma;         // Display Gamma
    u8 supportedFeatures;    // Feature Support (DPMS, color type, etc.)
    
    // Color characteristics (chromaticity coordinates)
    u8 redGreenLowBits;      // Red/Green low bits
    u8 blueWhiteLowBits;     // Blue/White low bits
    u8 redX;                 // Red X
    u8 redY;                 // Red Y
    u8 greenX;               // Green X
    u8 greenY;               // Green Y
    u8 blueX;                // Blue X
    u8 blueY;                // Blue Y
    u8 whiteX;               // White X
    u8 whiteY;               // White Y

    est_timings_t establishedTimings; // Established Timings (e.g., 720x400, 640x480, etc.)
    std_timing_t standardTimings[8];   // Standard Timing Information

    // Detailed Timing Descriptions (18 bytes each, 4 blocks)
    u8 detailedTimings[4][18];

    u8 extensionFlag;        // Number of extensions (typically 0 or 1)
    u8 checksum;             // Checksum for the 128-byte block
} EDID_t;


union Slot{
    // Normal EDID
    struct{
        EDID_t edid;
        u8 notUsed[128];
    };
    // Extended EDID
    struct{
        EDID_t extendedEdid;
        u8 extensionBlock[128];
    };
    u8 data[256];
};


#endif // EDID_STRUCT_H
