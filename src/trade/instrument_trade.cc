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

InstrumentTradeModule::InstrumentTradeModule(
    uint32_t moduleId, Container* pContainer,
    CThostFtdcTraderApi* pTraderApi) : Module(moduleId, pContainer) {
    _pTraderApi = pTraderApi;

    // TODO
}

Status InstrumentTradeModule::Dispatch(TaiyiMessage* msg) {
    switch (msg->cmd) {
        case PushTradeSignalReqCmd:
            return HandlePushTradeSignalReq(msg);
    }
    return StatusOK;
}

Status InstrumentTradeModule::HandlePushTradeSignalReq(TaiyiMessage* msg) {
    InstrumentTradeSignalReq* req = (InstrumentTradeSignalReq*)msg->data[0];
    AddTradeSignal(req->curMdNum-1, req->signal); // TODO 判断信号是否合法，通过idx时间戳等信息来判断
    return HandleTradeSignal();
}

Status InstrumentTradeModule::HandleTradeSignal() {
    TradeSignalOrderRecord* pRecord = GetLastRecord();
    switch (pRecord->signal) {
        case -1:
            return GoingShort();
        case 0:
            return ClosePosition();
        case 1:
            return GoingLong();
        default:
            LOG_ERROR("Invalid signal %d", pRecord->signal);
    }
    return StatusError;
}


/*--------------------------------------------------------------------------------------------------------
    当前持仓状态              未成交订单状态                   对应操作
----------------------------------------------------------------------------------------------------------
    净                        无挂单                           下空单
----------------------------------------------------------------------------------------------------------
    净                        有空单                           根据信号价格和挂单价格判断是否撤单重新下空单
----------------------------------------------------------------------------------------------------------
    净                        有多单                           撤多单，下空单
----------------------------------------------------------------------------------------------------------
    多头                      无挂单                           平多单，下空单
----------------------------------------------------------------------------------------------------------
    多头                      有空单                           平多单
----------------------------------------------------------------------------------------------------------
    多头                      有多单                           撤多单，平多单
----------------------------------------------------------------------------------------------------------
    空头                      无挂单                           无操作
----------------------------------------------------------------------------------------------------------
    空头                      有空单                           TODO
----------------------------------------------------------------------------------------------------------
    空头                      有多单                           撤多单
--------------------------------------------------------------------------------------------------------*/
Status InstrumentTradeModule::GoingShort() {
    switch (_pd) {
        case THOST_FTDC_PD_Net: // 净 => 当前无持仓？ TODO
            // TODO 判断挂单情况
            break;
        case THOST_FTDC_PD_Long:
            // TODO 判断挂单情况
            break;
        case THOST_FTDC_PD_Short:
            break;
    }
    return StatusError;
}

/*--------------------------------------------------------------------------------------------------------
    当前持仓状态              未成交订单状态                   对应操作
----------------------------------------------------------------------------------------------------------
    净                        无挂单                           下多单
----------------------------------------------------------------------------------------------------------
    净                        有空单                           撤空单，下多单
----------------------------------------------------------------------------------------------------------
    净                        有多单                           根据信号价格和挂单价格判断是否撤单重新下多单
----------------------------------------------------------------------------------------------------------
    多头                      无挂单                           无操作
----------------------------------------------------------------------------------------------------------
    多头                      有空单                           撤空单
----------------------------------------------------------------------------------------------------------
    多头                      有多单                           TODO
----------------------------------------------------------------------------------------------------------
    空头                      无挂单                           平空单，下多单
----------------------------------------------------------------------------------------------------------
    空头                      有空单                           撤空单，平空单
----------------------------------------------------------------------------------------------------------
    空头                      有多单                           平空单
--------------------------------------------------------------------------------------------------------*/
Status InstrumentTradeModule::GoingLong() {
    TradeSignalOrderRecord* lastRecord = GetLastRecord();

    // TODO 根据当前的持仓情况进行操作

    return StatusError;
}


/*--------------------------------------------------------------------------------------------------------
    当前持仓状态              未成交订单状态                   对应操作
----------------------------------------------------------------------------------------------------------
    净                        无挂单                           无操作
----------------------------------------------------------------------------------------------------------
    净                        有空单                           撤空单
----------------------------------------------------------------------------------------------------------
    净                        有多单                           撤多单
----------------------------------------------------------------------------------------------------------
    多头                      无挂单                           平多单
----------------------------------------------------------------------------------------------------------
    多头                      有空单                           撤空单，平多单
----------------------------------------------------------------------------------------------------------
    多头                      有多单                           撤多单，平多单
----------------------------------------------------------------------------------------------------------
    空头                      无挂单                           平空单
----------------------------------------------------------------------------------------------------------
    空头                      有空单                           撤空单，平空单
----------------------------------------------------------------------------------------------------------
    空头                      有多单                           撤多单，平空单
--------------------------------------------------------------------------------------------------------*/
Status InstrumentTradeModule::ClosePosition() {
    TradeSignalOrderRecord* lastRecord = GetLastRecord();

    // TODO 根据当前的持仓情况进行操作

    return StatusError;
}
