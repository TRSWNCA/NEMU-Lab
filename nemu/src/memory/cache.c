#include "nemu.h"
#include "memory/cache.h"
#include <time.h>
#include "burst.h"
#include <stdlib.h>

void init_cache() {
  int i;
  for (i = 0; i < CACHE_L1_SET_NUM * CACHE_L1_WAY_NUM; i++) {
    cache_L1[i].validVal = false;
  }
  for (i = 0; i < CACHE_L2_SET_NUM * CACHE_L2_WAY_NUM; i++) {
    cache_L2[i].dirtyVal = false;
    cache_L2[i].validVal = false;
  }
  return;
}

void ddr3_read_me(hwaddr_t addr, void* data);

// return whole index of way in cacheL1
int read_cache_L1(hwaddr_t addr) {
  uint32_t set_index = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L1_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L1_SET_BIT));
  // uint32_t block_start = ((addr >> CACHE_BLOCK_BIT) << CACHE_BLOCK_BIT);

  int way_index;
  int whole_begin_way_index = set_index * CACHE_L1_WAY_NUM;
  int whole_end_way_index = (set_index + 1) * CACHE_L1_WAY_NUM;
  for (way_index = whole_begin_way_index; way_index < whole_end_way_index; way_index++) {
    if (cache_L1[way_index].validVal && cache_L1[way_index].tag == tag) {
      // Hit!
      return way_index;
    }
  }
  // Hit loss!
  // go to cacheL2
  srand(time(0));
  int way_index_L2 = read_cache_L2(addr);
  way_index = whole_begin_way_index + rand() % CACHE_L1_WAY_NUM;
  memcpy(cache_L1[way_index].data, cache_L2[way_index_L2].data, CACHE_BLOCK_SIZE);

  cache_L1[way_index].validVal = true;
  cache_L1[way_index].tag = tag;
  return way_index;
}

void ddr3_write_me(hwaddr_t addr, void* data, uint8_t* mask);

// return whole index of way in cacheL2
int read_cache_L2(hwaddr_t addr) {
  uint32_t set_index = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L2_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L2_SET_BIT));
  uint32_t block_start = ((addr >> CACHE_BLOCK_BIT) << CACHE_BLOCK_BIT);

  int way_index;
  int whole_begin_way_index = set_index * CACHE_L2_WAY_NUM;
  int whole_end_way_index = (set_index + 1) * CACHE_L2_WAY_NUM;
  for (way_index = whole_begin_way_index; way_index < whole_end_way_index; way_index++) {
    if (cache_L2[way_index].validVal && cache_L2[way_index].tag == tag) {
      // Hit!
      return way_index;
    }
  }
  // Hit loss!
  srand(time(0));
  way_index = whole_begin_way_index + rand() % CACHE_L2_WAY_NUM;
  int i;
  if (cache_L2[way_index].validVal && cache_L2[way_index].dirtyVal) {
    // write down
    uint8_t tmp[BURST_LEN << 1];
    memset(tmp, 1, sizeof(tmp));
    uint32_t block_start_x = (cache_L2[way_index].tag << (CACHE_L2_SET_BIT + CACHE_BLOCK_BIT)) | (set_index << CACHE_BLOCK_BIT);
    for (i = 0; i < CACHE_BLOCK_SIZE / BURST_LEN; i++) {
      ddr3_write_me(block_start_x + BURST_LEN * i, cache_L2[way_index].data + BURST_LEN * i, tmp);
    }
  }
  for (i = 0; i < CACHE_BLOCK_SIZE / BURST_LEN; i++) {
    ddr3_read_me(block_start + BURST_LEN * i, cache_L2[way_index].data + BURST_LEN * i);
  }
  cache_L2[way_index].validVal = true;
  cache_L2[way_index].dirtyVal = false;
  cache_L2[way_index].tag = tag;
  return way_index;
}

void dram_write(hwaddr_t addr, size_t len, uint32_t data);

void write_cache_L1(hwaddr_t addr, size_t len, uint32_t data) {
  uint32_t set_index = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L1_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L1_SET_BIT));
  uint32_t block_bias = addr & (CACHE_BLOCK_SIZE - 1);
  int way_index;
  int whole_begin_way_index = set_index * CACHE_L1_WAY_NUM;
  int whole_end_way_index = (set_index + 1) * CACHE_L1_WAY_NUM;
  for (way_index = whole_begin_way_index; way_index < whole_end_way_index; way_index++) {
    if (cache_L1[way_index].validVal && cache_L1[way_index].tag == tag) {
      // Hit!
      // write through
      if (block_bias + len > CACHE_BLOCK_SIZE) {
        dram_write(addr, CACHE_BLOCK_SIZE - block_bias, data);
        memcpy(cache_L1[way_index].data + block_bias, &data, CACHE_BLOCK_SIZE - block_bias);
        write_cache_L2(addr, CACHE_BLOCK_SIZE - block_bias, data);
        write_cache_L1(addr + CACHE_BLOCK_SIZE - block_bias, len - (CACHE_BLOCK_SIZE - block_bias), data >> (CACHE_BLOCK_SIZE - block_bias));
      } else {
        dram_write(addr, len, data);
        memcpy(cache_L1[way_index].data + block_bias, &data, len);
        write_cache_L2(addr, len, data);
      }
      return;
    }
  }
  //  Hit loss!
  // not write allocate
  write_cache_L2(addr, len, data);
  return;
}

void write_cache_L2(hwaddr_t addr, size_t len, uint32_t data) {
  uint32_t set_index = ((addr >> CACHE_BLOCK_BIT) & (CACHE_L2_SET_NUM - 1));
  uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_L2_SET_BIT));
  uint32_t block_bias = addr & (CACHE_BLOCK_SIZE - 1);
  int way_index;
  int whole_begin_way_index = set_index * CACHE_L2_WAY_NUM;
  int whole_end_way_index = (set_index + 1) * CACHE_L2_WAY_NUM;
  for (way_index = whole_begin_way_index; way_index < whole_end_way_index; way_index++) {
    if (cache_L2[way_index].validVal && cache_L2[way_index].tag == tag) {
      // Hit!
      // write back
      cache_L2[way_index].dirtyVal = true;
      if (block_bias + len > CACHE_BLOCK_SIZE) {
        memcpy(cache_L2[way_index].data + block_bias, &data, CACHE_BLOCK_SIZE - block_bias);
        write_cache_L2(addr + CACHE_BLOCK_SIZE - block_bias, len - (CACHE_BLOCK_SIZE - block_bias), data >> (CACHE_BLOCK_SIZE - block_bias));
      } else {
        memcpy(cache_L2[way_index].data + block_bias, &data, len);
      }
      return;
    }
  }
  //  Hit loss!
  // write allocate
  way_index = read_cache_L2(addr);
  cache_L2[way_index].dirtyVal = true;
  memcpy(cache_L2[way_index].data + block_bias, &data, len);
  return;
}
