/*
 * ctp_service.cc
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#include "ctp_service.h"
#include "md_spi.h"

CtpService::CtpService(Config* cfg) {
    _cfg = cfg;

    _pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    DBG_ASSERT(_pMdApi);
    TaiyiMdSpi* pMdSpi  = new TaiyiMdSpi(cfg, _pMdApi);
    DBG_ASSERT(pMdSpi);
    _pMdApi->RegisterSpi(pMdSpi);
    strcpy(_pMdFrontAddr, cfg->mdFrontAddr.c_str());
    _pMdApi->RegisterFront(_pMdFrontAddr);

    // TODO create traderApi
}

void CtpService::Start() {
    _pMdApi->Init();
    // TODO _pTraderApi->Init();

    _pMdApi->Join();
    // TODO _pTraderApi->Join();
}
