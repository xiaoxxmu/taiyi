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

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#include "module.h"

struct TradeSignalOrderRecord {
    uint32_t mdIdx;
    int signal;
    CThostFtdcInputOrderField* order; // 报单
    CThostFtdcInputOrderActionField* action; // 撤单
};

class InstrumentTradeModule : public Module {
  public:
    InstrumentTradeModule(uint32_t moduleId, Container* pContainer, CThostFtdcTraderApi* pTraderApi);
    ~InstrumentTradeModule() {} // TODO

  public:
    Status Dispatch(TaiyiMessage* msg);

  private:
    Status HandlePushTradeSignalReq(TaiyiMessage* msg);

  private:
    std::string _instrumentId;
    TradeSignalOrderRecord* _ppRecords[MD_NUM_PER_INSTRUMENT]; // 每个行情对应一次交易信号和订单信息
    uint32_t _recordNum;
    uint32_t _commitNum; // 收到订单或撤单回报的数量，即当前完成交易的位置

    TThostFtdcPosiDirectionType pd; // 持仓方向
    TThostFtdcVolumeType volumeNum; // 持仓数量

  private:
    void AddTradeSignal(uint32_t mdIdx, int signal) {
        TradeSignalOrderRecord* pRecord = _ppRecords[_recordNum++];
        pRecord->mdIdx = mdIdx;
        pRecord->signal = signal;
    }
    TradeSignalOrderRecord* GetLastRecord() { return _recordNum ? _ppRecords[_recordNum-1] : NULL; }
    TradeSignalOrderRecord* GetPrevRecord() { return _recordNum > 1 ? _ppRecords[_recordNum-2] : NULL; }

  private:
    Status HandleTradeSignal();
    Status GoingShort(); // 做空
    Status GoingLong(); // 做多
    Status ClosePosition(); // 平仓

  private:
    CThostFtdcTraderApi* _pTraderApi;
};
