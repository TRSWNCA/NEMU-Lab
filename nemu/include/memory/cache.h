#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

#define Cache_Block_Bit 6
#define Cache_Block_Size 64
#define Cache_L1_Cap 64 * 1024
#define Cache_L1_Way_Bit 3
#define Cache_L1_Way_Num 8
#define Cache_L1_Set_Bit 7
#define Cache_L1_Set_Num 128

typedef struct{
    uint8_t data[Cache_Block_Size];
    uint32_t tag;
    bool valid_value;
}Cache_L1_Way;

Cache_L1_Way cache_L1[Cache_L1_Set_Num * Cache_L1_Way_Num];

void init_cache();

int read_Cache_L1(hwaddr_t);

void write_Cache_L1(hwaddr_t, size_t, uint32_t);

/*
Cache L2
cache block 存储空间的大小为 64B
cache 存储空间的大小为 4MB
16-way set associative
标志位包括 valid bit 和 dirty bit
替换算法采用随机方式
write back
write allocate
*/

#define Cache_L2_Cap 4 * 1024 * 1024
#define Cache_L2_Way_Bit 4
#define Cache_L2_Way_Num 16
#define Cache_L2_Set_Bit 12
#define Cache_L2_Set_Num 4096

typedef struct{
    uint8_t data[Cache_Block_Size];
    uint32_t tag;
    bool valid_value;
    bool dirty_value;
}Cache_L2_Way;

Cache_L2_Way cache_L2[Cache_L2_Set_Num * Cache_L2_Way_Num];

void init_cache();

int read_Cache_L1(hwaddr_t);
int read_Cache_L2(hwaddr_t);

void write_Cache_L1(hwaddr_t, size_t, uint32_t);
void write_Cache_L2(hwaddr_t, size_t, uint32_t);
#endif