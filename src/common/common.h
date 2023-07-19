#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>



#ifdef __cplusplus
extern "C" {
#endif

uint8_t* PutByteStream(uint8_t* stream, uint64_t srcValue, size_t numBytes, uint32_t* offset);

uint64_t GetByteStream(uint8_t* stream, size_t numBytes, uint32_t* offset);

uint8_t* SaveInBigEndian(uint8_t* array, uint64_t value, size_t numBytes);

uint64_t ExtractFromBigEndian(uint8_t* array, size_t numBytes);



#ifdef __cplusplus
}
#endif

#endif