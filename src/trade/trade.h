/*
 * trade.h
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

class TradeModule : public Module {
  public:
    TradeModule(uint32_t moduleId, Container* pContainer) : Module(moduleId, pContainer) {} // TODO
    ~TradeModule() {} // TODO

  public:
    Status Dispatch(Message* msg);

  private:
    std::string _instrumentId;
    // TODO 行情记录
};
