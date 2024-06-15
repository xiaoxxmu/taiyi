/*
 * mem.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define TAIYI_MALLOC malloc
#define TAIYI_FREE free

class TaiyiMemPool {
  public:
    TaiyiMemPool(uint32_t itemNum, uint32_t itemSize);
    ~TaiyiMemPool() {} // TODO

    void* Zalloc();

  private:
    void** _ppMemAddrs;
    uint32_t _itemNum;
    uint32_t _itemSize;
    uint32_t _allocIdx;
};
