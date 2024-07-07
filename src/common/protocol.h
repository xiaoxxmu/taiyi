/*
 * protocol.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include "common.h"

enum TaiyiCmdCode {
    PushMarketDataReqCmd = 1,
    PushTradeSignalReqCmd,

    OrderInsertRspErrorCmd,
    OrderTradedRspCmd,
};

// 由InstrumentMdModule发送给InstrumentTradeModule的信号处理请求
struct InstrumentTradeSignalReq {
    uint32_t curMdNum;
    int signal;
    double price;
    int volume;
};

struct OrderInsertErrorRsp {
    int nRequestId;
    int retcode;
    OrderRefType ref;
};

struct OrderTradedRsp {
    OrderRefType ref;

    int volumeTotal; // 总量
    double price;
     int volumeTraded; // 已完成数量

    OrderTradeStatusType tradeStatus; // 交易状态
};
