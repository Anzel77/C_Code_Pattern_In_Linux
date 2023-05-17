#include "common.h"

#include <stdio.h>

void PutByteStream(uint8_t* stream, uint64_t srcValue, size_t numBytes, uint32_t* offset) {
    for (int i = numBytes - 1; i >= 0; i--) {
        *(stream + *offset) = (uint8_t)(srcValue >> (8 * i));
        (*offset) += 1;
    }
}

void GetByteStream(uint8_t* stream, uint64_t* const dstValue, size_t numBytes, uint32_t* offset) {
    for (int i = numBytes - 1; i >=0; i--) {
        *dstValue |= *(stream + *offset) << (8 * i);
        (*offset) += 1;
    }
}

void SaveInBigEndian(uint8_t* array, uint64_t value, size_t numBytes) {
    for (int i = 0; i < numBytes; i++) {
        array[numBytes - 1 - i] = (value >> (8 * i)) & 0xFF;
    }
}

void ExtractFromBigEndian(uint8_t* array, uint64_t* value, size_t numBytes) {
    for (int i = 0; i < numBytes; i++) {
        *value |= (array[i] << (numBytes - 1 - i)) & 0xFF;
    }
}
