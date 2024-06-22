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

enum TaiyiCmdCode {
    PushMarketDataReqCmd = 1,
    PushTradeSignalReqCmd,

    OrderInsertRspCmd,
};

// 由InstrumentMdModule发送给InstrumentTradeModule的信号处理请求
struct InstrumentTradeSignalReq {
    int signal;
    uint32_t curMdNum;
};

struct OrderInsertRsp {
    int nRequestId;
    int retcode;
};
