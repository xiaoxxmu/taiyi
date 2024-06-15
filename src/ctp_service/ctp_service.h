/*
 * ctp_service.h
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#pragma once

#include "ThostFtdcMdApi.h"

#include "ctp_service_intf.h"

class CtpService : public ICtpService {
  public:
    CtpService(Config* cfg);
    ~CtpService() {} // TODO

    void Start();

  private:
    Config* _cfg;
    char _pMdFrontAddr[256];

  private:
    CThostFtdcMdApi* _pMdApi;
    // TODO tradeApi
};
