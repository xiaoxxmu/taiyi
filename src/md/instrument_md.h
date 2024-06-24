/*
 * instrument_md.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <string>

#include "proc.h"
#include "module.h"
#include "config.h"
#include "mem.h"
#include "ctp_data.h"

class InstrumentMdModule : public Module {
  public:
    InstrumentMdModule(uint32_t moduleId, Container* pContainer, InstrumentConfig* cfg);
    ~InstrumentMdModule() {} // TODO

  public:
    Status Dispatch(TaiyiMessage* msg);

  private:
    Status HandlePushMarketDataReq(TaiyiMessage* msg);
    int DoStrategy();
    void SendTradeSignal(int signal);

  private:
    InstrumentConfig* _cfg;
    InstrumentLocation* _location;

  private:
    InstrumentLocation* GetLocation() {
        if (!_location) {
            _location = TaiyiMain()->GetInstrumentLocation(_cfg->instrumentId);
        }
        return _location;
    }

  private:
    MarketData* _pMds[MD_NUM_PER_INSTRUMENT]; // 全缓存行情信息
    uint32_t _mdNum;

  private:
    uint32_t GetCurMdNum() { return _mdNum; }

  private:
    TaiyiMemPool* _pMsgPool;
    TaiyiMemPool* _pTradeReqPool;
};
