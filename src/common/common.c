#include "common.h"

#include <stdio.h>
#include <stdint.h>

uint8_t* PutByteStream(uint8_t* stream, uint64_t srcValue, size_t numBytes, uint32_t* offset) {
    for (int i = numBytes - 1; i >= 0; i--) {
        *(stream + *offset) = (uint8_t)(srcValue >> (8 * i));
        (*offset) += 1;
    }

    return stream;
}

uint64_t GetByteStream(uint8_t* stream, size_t numBytes, uint32_t* offset) {
    uint64_t result = 0;

    if (stream == NULL || numBytes > 8) return -1;

    for (size_t i = 0; i < numBytes; i++) {
        result = (result << 8) | stream[*offset];
        (*offset) += 1;
    }

    return result;
}

uint8_t* SaveInBigEndian(uint8_t* array, uint64_t value, size_t numBytes) {
    for (int i = 0; i < numBytes; i++) {
        array[numBytes - 1 - i] = (value >> (8 * i)) & 0xFF;
    }

    return array;
}

uint64_t ExtractFromBigEndian(uint8_t* array, size_t numBytes) {
    uint64_t result = 0;

    for (size_t i = 0; i < numBytes; i++) {
        result = (result << 8) | array[i];
    }

    return result;
}
