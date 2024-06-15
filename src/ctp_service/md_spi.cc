/*
 * md_spi.cc
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#include <string>

#include "protocol.h"
#include "log.h"
#include "proc.h"

#include "md_spi.h"

void TaiyiMdSpi::OnFrontConnected() {
    LOG_INFO("OnFrontConnected success, begin Login.");

    CThostFtdcReqUserLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
    strcpy(loginReq.BrokerID, _brokerId);
    strcpy(loginReq.UserID, _investerId);
    strcpy(loginReq.Password, _investerPwd);

    int ret = _pMdApi->ReqUserLogin(&loginReq, 0);
    if (!ret) {
        LOG_INFO("send ReqUserLogin success");
    } else {
        LOG_ERROR("send ReqUserLogin failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnFrontDisconnected(int nReason) {
    LOG_ERROR("OnFrontDisconnected nReason: %d", nReason);
    // TODO 是否需要做一些异常处理，有可能会少收到一些行情，通知策略做一次的处理
}

void TaiyiMdSpi::OnHeartBeatWarning(int nTimeLapse) {
    LOG_ERROR("OnHeartBeatWarning nTimeLapse: %d", nTimeLapse);
    // TODO 是否需要做一些异常处理，有可能会少收到一些行情，通知策略做一次的处理
}

void TaiyiMdSpi::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspUserLogin ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    LOG_DEBUG("OnRspUserLogin success, send SubscribeMarketData req");

    int ret = _pMdApi->SubscribeMarketData(_ppInstrumentIds, _instrumentNum);
    if (!ret) {
        LOG_INFO("send SubscribeMarketData req success");
    } else {
        LOG_ERROR("send SubscribeMarketData req failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspUserLogout ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    LOG_DEBUG("OnRspUserLogout success, BrokerID: %s, UserID: %s", pUserLogout->BrokerID, pUserLogout->UserID);
}

void TaiyiMdSpi::OnRspQryMulticastInstrument(
    CThostFtdcMulticastInstrumentField *pMulticastInstrument,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    // TODO
}

void TaiyiMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspError ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    // TODO 异常处理
}

void TaiyiMdSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspSubMarketData ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    // TODO
}

void TaiyiMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspUnSubMarketData ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        // TODO 异常处理
    }
    LOG_DEBUG("OnRspUnSubMarketData success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspSubForQuoteRsp ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        // TODO 异常处理
    }
    LOG_DEBUG("OnRspSubForQuoteRsp success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspSubForQuoteRsp ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        // TODO 异常处理
    }
    LOG_DEBUG("OnRspSubForQuoteRsp success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {

    // TODO 先判断是否是合法的行情，如果不是合法的行情，则丢弃

    std::string instrumentId = pDepthMarketData->InstrumentID;

    InstrumentLocation* location = TaiyiMain()->GetInstrumentLocation(instrumentId);
    if (!location) {
        LOG_ERROR("InstrumentId %s NOT FOUND", instrumentId.c_str());
        return ;
    }

    TaiyiMessage* msg = (TaiyiMessage*)_pMsgPool->Zalloc();
    DBG_ASSERT(msg);
    msg->cmd = PushMarketDataReq;
    msg->srcContainerId = _containerId;
    msg->srcModuleId = _moduleId;
    msg->dstContainerId = location->mdContainerId;
    msg->dstModuleId = location->mdModuleId;

    CThostFtdcDepthMarketDataField* md = (CThostFtdcDepthMarketDataField*)_pMdPool->Zalloc();
    memcpy(md, pDepthMarketData, sizeof(CThostFtdcDepthMarketDataField));
    msg->data[0] = md;

    Container* container = TaiyiMain()->GetContainer(location->mdContainerId);
    DBG_ASSERT(container);

    if (container->Push(msg)) {
        LOG_ERROR("Push instrument %s market data to container %d module %d failed",
            location->instrumentId.c_str(), location->mdContainerId, location->mdModuleId);
        // TODO 异常或重试队列？
    }
}

void TaiyiMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
    LOG_DEBUG("OnRtnForQuoteRsp TradingDay: %s ExchangeID: %s InstrumentID: %s ForQuoteSysID: %s",
        pForQuoteRsp->TradingDay, pForQuoteRsp->ExchangeID, pForQuoteRsp->InstrumentID, pForQuoteRsp->ForQuoteSysID);
}

/////////////////////////////////////////////////////////

TaiyiMdSpi::TaiyiMdSpi(Config* cfg, CThostFtdcMdApi* pMdApi) {
    _pMdApi = pMdApi;
    _containerId = 0;
    _moduleId = 0;

    strcpy(_brokerId, cfg->brokerId.c_str());
    strcpy(_investerId, cfg->investerId.c_str());
    strcpy(_investerPwd, cfg->investerPwd.c_str());

    _instrumentNum = int(cfg->instruments.size());
    _ppInstrumentIds = (char**)TAIYI_MALLOC(sizeof(char*)*_instrumentNum);
    DBG_ASSERT(_ppInstrumentIds);
    for (int i = 0; i < _instrumentNum; i++) {
        _ppInstrumentIds[i] = (char*)TAIYI_MALLOC(sizeof(TThostFtdcInstrumentIDType));
        DBG_ASSERT(_ppInstrumentIds[i]);
        strcpy(_ppInstrumentIds[i], cfg->instruments[i]->instrumentId.c_str());
    }

    uint32_t memPoolSize = cfg->instruments.size() * MD_NUM_PER_INSTRUMENT;
    _pMsgPool = new TaiyiMemPool(memPoolSize, sizeof(TaiyiMessage));
    DBG_ASSERT(_pMsgPool);
    _pMdPool = new TaiyiMemPool(memPoolSize, sizeof(CThostFtdcDepthMarketDataField));
    DBG_ASSERT(_pMdPool);
}
