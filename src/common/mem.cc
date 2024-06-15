/*
 * mem.cc
 *
 * Created on: 20240613
 *      Author xiaoxxmu
 *
 */

#include "mem.h"
#include "common.h"

TaiyiMemPool::TaiyiMemPool(uint32_t itemNum, uint32_t itemSize) {
    _itemNum = itemNum;
    _itemSize = itemSize;
    _allocIdx = 0;

    _ppMemAddrs = (void**)TAIYI_MALLOC(sizeof(void*)*itemNum);
    DBG_ASSERT(_ppMemAddrs);
    for (uint32_t i = 0; i < itemNum; i++) {
        void* item = (void*)TAIYI_MALLOC(itemSize);
        DBG_ASSERT(item);
        memset(item, 0, itemSize);
        _ppMemAddrs[i] = item;
    }
}

void* TaiyiMemPool::Zalloc() {
    if (_allocIdx < _itemNum) {
        return _ppMemAddrs[_allocIdx++];
    } else {
        void* item = (void*)TAIYI_MALLOC(_itemSize);
        if (item) {
            memset(item, 0, _itemSize);
        }
        return item;
    }
}
