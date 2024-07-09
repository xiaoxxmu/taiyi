
#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cassert>

#include <thread>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

#define DBG_ASSERT(expr) assert(expr)

#define MD_NUM_PER_SECOND (2)                           // 每秒行情数
#define MD_NUM_PER_HOUR ((MD_NUM_PER_SECOND)*3600)      // 每小时行情数
#define MAX_TRADE_WINDOW_HOUR (5)                       // 交易窗口持续时间，5小时

#define MAX_ORDER_NUM_PER_INSTRUMENT (4000) // 每个合约在一个交易周期内最多的订单次数


#define MD_THREAD_CORE_ID (0)
#define TRADE_THREAD_CORE_ID (1)
#define COMMON_THREAD_CORE_ID (2)

static inline bool BindCore(uint32_t core_id) {
    uint32_t cores = std::thread::hardware_concurrency();
    if (core_id >= cores) { return false; }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);

    return (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) >= 0);
}
