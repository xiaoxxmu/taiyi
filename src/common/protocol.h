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
    PushMarketDataReq = 1,
    PushTradeSignalReq
};

// 由InstrumentMdModule发送给InstrumentTradeModule的信号处理请求
struct InstrumentTradeSignalReq {
    void* pLastMarketData;
    int signal;
};
