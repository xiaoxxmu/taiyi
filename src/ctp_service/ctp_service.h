/*
 * ctp_service.h
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#pragma once

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#include "md_spi.h"
#include "trade_spi.h"

#include "ctp_service_intf.h"

class CtpService : public ICtpService {
  public:
    CtpService(Config* cfg);
    ~CtpService() {} // TODO

    void Start();

  public:
    Status CancelOrder(OrderRefType orderRef);
    OrderRefType InsertOrder(std::string instrumentId, Order* order);

  private:
    Config* _cfg;
    char _pMdFrontAddr[256];
    char _pTraderFrontAddr[256];
    OrderRefType _ref;
    int _nReqId;

  private:
    int GetReqId() { return _nReqId++; }

  private:
    CThostFtdcMdApi* _pMdApi;
    CThostFtdcTraderApi* _pTraderApi;

    TaiyiMdSpi* _pMdSpi;
    TaiyiTraderSpi* _pTraderSpi;
};
