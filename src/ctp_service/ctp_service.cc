/*
 * ctp_service.cc
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#include "ctp_service.h"
#include "md_spi.h"
#include "trade_spi.h"

CtpService::CtpService(Config* cfg) {
    _cfg = cfg;

    _pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    DBG_ASSERT(_pMdApi);
    TaiyiMdSpi* pMdSpi  = new TaiyiMdSpi(cfg, _pMdApi);
    DBG_ASSERT(pMdSpi);
    _pMdApi->RegisterSpi(pMdSpi);
    strcpy(_pMdFrontAddr, cfg->mdFrontAddr.c_str());
    _pMdApi->RegisterFront(_pMdFrontAddr);

    _pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // TODO FlowPath
    DBG_ASSERT(_pTraderApi);
    TaiyiTraderSpi* pTraderSpi = new TaiyiTraderSpi(cfg, _pTraderApi);
    DBG_ASSERT(pTraderSpi);
    _pTraderApi->RegisterSpi(pTraderSpi);
    strcpy(_pTraderFrontAddr, cfg->traderFrontAddr.c_str());
    _pTraderApi->RegisterFront(_pTraderFrontAddr);
    _pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    _pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
}

void CtpService::Start() {
    _pMdApi->Init();
    _pTraderApi->Init();

    _pMdApi->Join();
    _pTraderApi->Join();
}
