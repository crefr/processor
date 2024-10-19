#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "hash.h"

static uint32_t MurMur32Scramble(uint32_t datablock);

static uint32_t MurMur32Scramble(uint32_t datablock) {
    datablock *= 0xcc9e2d51;
    datablock = (datablock << 15) | (datablock >> 17);
    datablock *= 0x1b873593;
    return datablock;
}

uint32_t MurMur32Hash(const void * data, size_t len, uint32_t seed)
{
    const uint8_t * databytes = (const uint8_t *) data;
    uint32_t h = seed;
    uint32_t curblock = 0;

    //blocks by 4 bytes
    for (size_t count = len / sizeof(uint32_t); count != 0; count--){
        memcpy(&curblock, databytes, sizeof(uint32_t));
        databytes += sizeof(uint32_t);
        h ^= MurMur32Scramble(curblock);
        h = (h << 13) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    //the rest of bytes (least than 4)
    curblock = 0;
    for (size_t index = len % sizeof(uint32_t); index != 0; index--){
        curblock <<= sizeof(uint8_t);
        curblock |= databytes[index - 1];
    }
    h ^= MurMur32Scramble(curblock);

    //finalizing
	h ^= (uint32_t) len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
    return h;
}
