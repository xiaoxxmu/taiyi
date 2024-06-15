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

#include "module.h"

class InstrumentTradeModule : public Module {
  public:
    InstrumentTradeModule(uint32_t moduleId, Container* pContainer) : Module(moduleId, pContainer) {} // TODO
    ~InstrumentTradeModule() {} // TODO

  public:
    Status Dispatch(TaiyiMessage* msg);

  private:
    Status HandlePushTradeSignalReq(TaiyiMessage* msg);

  private:
    std::string _instrumentId;
    // TODO 交易记录
};
