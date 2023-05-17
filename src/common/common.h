#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Put data into a byte stream in big-endian form
 * 
 * @param stream [out] The pointer to stream where the value will be put into
 * @param srcValue [in] The value will be put into stream
 * @param numBytes [in] The number of bytes that need to be placed
 * @param offset [out] offset
 */
void PutByteStream(uint8_t* stream, uint64_t srcValue, size_t numBytes, uint32_t* offset);

/**
 * @brief Get value from a big-endian byte stream
 * 
 * @param stream [in] The pointer to stream where the value will be got out of
 * @param dstValue [out] The pointer to saving value
 * @param numBytes [in] The number of bytes that need to be got
 * @param offset [out] offset
 */
void GetByteStream(uint8_t* stream, uint64_t* const dstValue, size_t numBytes, uint32_t* offset);

/**
 * @brief Store values ​​in big-endian format into an array
 * 
 * @param array [out] 
 * @param value [in] 
 * @param numBytes [in] 
 */
void SaveInBigEndian(uint8_t* array, uint64_t value, size_t numBytes);

/**
 * @brief Extract numeric values from arrays stored in big-endian form
 * 
 * @param array [in] 
 * @param value [out] 
 * @param numBytes [in] 
 */
void ExtractFromBigEndian(uint8_t* array, uint64_t* value, size_t numBytes);



#ifdef __cplusplus
}
#endif

#endif