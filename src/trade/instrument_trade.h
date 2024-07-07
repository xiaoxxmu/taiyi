/*
 * instrument_trade.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <string>
#include <map>

#include "module.h"
#include "ctp_data.h"
#include "../ctp_service/ctp_service_intf.h"
#include "mem.h"


struct TradeInfo {
    uint32_t mdIdx;
    TradeSignal signal;
    std::vector<Order*> orders; // 当前交易信号下产生的订单
    std::vector<OrderAction*> actions; // 当前交易信号下产生的撤单
};

class InstrumentTradeModule : public Module {
  public:
    InstrumentTradeModule(uint32_t moduleId, Container* pContainer, ICtpService* pCtpService);
    ~InstrumentTradeModule() {} // TODO

  public:
    Status Dispatch(TaiyiMessage* msg);

  private:
    Status HandlePushTradeSignalReq(TaiyiMessage* msg);
    Status HandleOrderInsertErrorRsp(TaiyiMessage* msg);
    Status HandleOrderTradedRsp(TaiyiMessage* msg);

  private:
    std::string _instrumentId;
    TradeInfo* _ppTradeInfo[MD_NUM_PER_INSTRUMENT]; // 每个行情对应一次交易信息
    uint32_t _tradeInfoNum;
    Position _position; // 当前持仓
    std::map<OrderRefType, Order*> _unfinishedOrder; // 当前未完成的订单信息

  private:
    void AddTradeSignal(uint32_t mdIdx, TradeSignal& signal) {
        TradeInfo* pInfo = _ppTradeInfo[_tradeInfoNum++];
        pInfo->mdIdx = mdIdx;
        pInfo->signal = signal;
        // TODO
    }
    TradeInfo* GetLastTradeInfo() { return _tradeInfoNum ? _ppTradeInfo[_tradeInfoNum-1] : NULL; }

  private:
    Status HandleTradeSignal();
    Status GoingShort(); // 做空
    Status GoingLong(); // 做多
    Status ClosePosition(); // 平仓

    Status InsertOrder(OrderDirectionType direction, OrderCombOffsetType combOffset, int volume);
    Status CancelOrder(OrderRefType ref);

  private:
    ICtpService* _pCtpService;

  private:
    TaiyiMemPool* _pOrderPool;
    TaiyiMemPool* _pOrderActionPool;
};
