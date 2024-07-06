/*
 * ctp_service_intf.h
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#pragma once

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#include "common.h"
#include "config.h"
#include "ctp_data.h"

class ICtpService {
  public:
    static ICtpService* CreateCtpService(Config* cfg);

  public:
    virtual void Start() = 0;

  public:
    virtual Status CancelOrder(OrderRefType orderRef) = 0; // TODO
    virtual OrderRefType InsertOrder(std::string instrumentId, Order* order) = 0; // TODO
};
