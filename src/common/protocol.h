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
    uint32_t curMdNum;
    int signal;
    double price;
    int volume;
};

struct InsertOrderErrRsp {
    uint32_t orderRef;
    int nRequestId;
    int retcode;

    // TODO
};

struct OrderTradeRsp {
    // TODO 考虑与InsertOrderErrRsp合成一个请求
};