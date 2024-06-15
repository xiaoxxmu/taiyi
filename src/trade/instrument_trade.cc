/*
 * instrument_trade.cc
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#include "instrument_trade.h"

#include "log.h"
#include "message.h"
#include "protocol.h"

Status InstrumentTradeModule::Dispatch(TaiyiMessage* msg) {
    switch (msg->cmd) {
        case PushTradeSignalReq:
            return HandlePushTradeSignalReq(msg);
    }
    return StatusOK;
}

Status InstrumentTradeModule::HandlePushTradeSignalReq(TaiyiMessage* msg) {
    InstrumentTradeSignalReq* req = (InstrumentTradeSignalReq*)msg->data[0];
    // TODO 处理交易信息
    return StatusError;
}
