/*
 * message.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include "common.h"

struct TaiyiMessage {
    uint32_t srcContainerId;
    uint32_t srcModuleId;
    uint32_t dstContainerId;
    uint32_t dstModuleId;

    CmdCode cmd;
    void* data[MAX_DATA_BUF_NUM];
};
