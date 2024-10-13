#ifndef MYSTRING_H
#define MYSTRING_H


#include <stdint.h>




// Calculate the length of a string, until the first '\0' is found
uint16_t len(const char* str);

// Calculate the length of a string, until the first '\0' is found or the maxLen is reached. Returns maxLen if the string is longer than maxLen
uint16_t len(const char* str, uint16_t maxLen);

// Return first position of a character in a string
uint16_t find(const char* str, char c);

// Insert a char[] into another char[] at a specific position.
void insert(char* str, const char* insertStr, uint16_t pos);

// Replace a char[] into another char[] at a specific position.
void replace(char* str, const char* insertStr, uint16_t pos);

// Append a char[] to another char[]
void append(char* str, const char* appendStr);

// outStr must have a length of 2. No \0 is added at the end
void byteToHexString(char* outStr, const uint8_t inByte);

// Converts a uint32_t to a string. str must have a length of min 11. First char is the most significant digit. At the end a '\0' is added.
void uintToString(char* str, uint32_t inUint);

// Converts a int32_t to a string. str must have a length of min 12. First char is the most significant digit or -. At the end a '\0' is added
void intToString(char* str, int32_t inInt);

bool startsWith(const char* str, const char* start);

#endif // MYSTRING_H