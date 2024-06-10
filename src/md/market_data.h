/*
 * market_data.h
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

class MarketDataModule : public Module {
  public:
    MarketDataModule(uint32_t moduleId, Container* pContainer) : Module(moduleId, pContainer) {} // TODO
    ~MarketDataModule() {} // TODO

  public:
    Status Dispatch(Message* msg);

  private:
    Status HandlePushMarketDataReq(Message* msg);

  private:
    std::string _instrumentId;
    // TODO 行情记录
};
