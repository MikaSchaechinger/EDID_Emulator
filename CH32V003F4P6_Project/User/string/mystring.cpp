#include "mystring.h"


using namespace std;

uint16_t len(const char* str){
    uint16_t i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}


uint16_t len(const char* str, uint16_t maxLen){
    uint16_t i = 0;
    while(str[i] != '\0' && i < maxLen){
        i++;
    }
    return i;
}


uint16_t find(const char* str, char c){
    uint16_t i = 0;
    while(str[i] != '\0'){
        if (str[i] == c){
            return i;
        }
        i++;
    }
    return i;
}


void insert(char* str, const char* insertStr, uint16_t pos){
    uint16_t lenStr = len(str);
    uint16_t lenInsert = len(insertStr);
    // Check if the position is valid
    if (pos > lenStr){
        return;
    }
    // Move the string to the right together with the '\0' at the end
    for (uint16_t i = lenStr; i >= pos; i--){
        str[i + lenInsert] = str[i];
    }
    // Insert the new string, without the '\0' at the end
    for (uint16_t i = 0; i < lenInsert; i++){
        str[pos + i] = insertStr[i];
    }
}


void replace(char* str, const char* insertStr, uint16_t pos){
    uint16_t lenStr = len(str);
    uint16_t lenInsert = len(insertStr);
    if (pos > lenStr){
        return;
    }
    for (uint16_t i = 0; i < lenInsert; i++){
        str[pos + i] = insertStr[i];
    }
}


void append(char* str, const char* appendStr){
    uint16_t lenAppend = len(appendStr);
    uint16_t i = 0;

    // Get index of the first '\0'
    while(str[i] != '\0'){
        i++;
    }
    str += i;   // Move the pointer to the first '\0'
    for (uint16_t i = 0; i < lenAppend; i++){
        str[i] = appendStr[i];
    }
}


void byteToHexString(char* outStr, const uint8_t inByte){
    uint8_t high = inByte >> 4;
    uint8_t low = inByte & 0x0F;
    
    if (high < 10)      outStr[0] = high + '0';
    else                outStr[0] = high - 10 + 'A';
    
    if (low < 10)       outStr[1] = low + '0';
    else                outStr[1] = low - 10 + 'A';
}    


void uintToString(char* str, uint32_t inUint) {
    if (inUint == 0) {         // Spezieller Fall für 0
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    uint16_t i = 0;
    uint32_t temp = inUint;
    while (temp > 0) {
        temp /= 10;
        i++;
    }

    str[i] = '\0';  // String Terminierung
    i--;

    while (inUint > 0) {
        str[i] = inUint % 10 + '0';
        inUint /= 10;
        i--;
    }

}



void intToString(char* str, int32_t inInt){
    if (inInt < 0){
        str[0] = '-';
        uintToString(str + 1, -inInt);
        return;
    }
    uintToString(str, inInt);
}






bool startsWith(const char* str, const char* prefix){
    uint16_t i = 0;
    while(prefix[i] != '\0' && str[i] != '\0'){
        if (str[i] != prefix[i]){
            return false;
        }
        i++;
    }
    return true; 
}
