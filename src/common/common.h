/*
 * common.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <cassert>

#define DBG_ASSERT(expr) assert(expr)

#define MAX_DATA_BUF_NUM (5)

#define TAIYI_STORE_CONTAINER_ID (1)
#define TAIYI_MD_CONTAINER_ID ((TAIYI_STORE_CONTAINER_ID)+1)
#define TAIYI_MAX_MD_CONTAINER_NUM (8)
#define TAIYI_TRADE_CONTAINER_ID ((TAIYI_MD_CONTAINER_ID)+(TAIYI_MAX_MD_CONTAINER_NUM))

#define TAIYI_STORE_MODULE_ID (1)
#define TAIYI_MD_MODULE_ID ((TAIYI_STORE_MODULE_ID)+1)
#define TAIYI_MAX_MD_MODULE_NUM (4096)
#define TAIYI_TRADE_MODULE_ID ((TAIYI_MD_MODULE_ID)+(TAIYI_MAX_MD_MODULE_NUM))
#define TAIYI_MAX_TRADE_MODULE_NUM (TAIYI_MAX_MD_MODULE_NUM)

#define MD_NUM_PER_INSTRUMENT (6*7200)  // 单个交易周期每个合约有效行情个数，按6小时算，多预留一些
#define ORDER_NUM_PER_INSTRUMENT (4000) // 每个合约在一个交易周期内最多的订单次数

typedef uint32_t CmdCode;

typedef int Status;
#define StatusOK (0)
#define StatusError (-1)
#define StatusNotFound (1)
#define StatusNoMemory (2)
