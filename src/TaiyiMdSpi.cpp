#include "TaiyiModule.h"
#include "TaiyiMdSpi.h"
#include "logs.h"

void TaiyiMdSpi::OnFrontConnected() {
    LogInfo("OnFrontConnected success, begin Login.");

    TaiyiModule *tym = TaiyiModule::GetInstance();

    CThostFtdcReqUserLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
    strcpy(loginReq.BrokerID, tym->GetBrokerID());
    strcpy(loginReq.UserID, tym->GetUserID());
    strcpy(loginReq.UserID, tym->GetInvesterID());

    int ret = tym->GetTaiyiMdApi()->ReqUserLogin(&loginReq, 0);
    if (!ret) {
        LogInfo("send ReqUserLogin success");
    } else {
        LogError("send ReqUserLogin failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnFrontDisconnected(int nReason) {
    LogError("OnFrontDisconnected nReason: %d", nReason);
    // TODO 是否需要做一些异常处理，有可能会少收到一些行情，通知策略做一次的处理
}

void TaiyiMdSpi::OnHeartBeatWarning(int nTimeLapse) {
    LogError("OnHeartBeatWarning nTimeLapse: %d", nTimeLapse);
    // TODO 是否需要做一些异常处理，有可能会少收到一些行情，通知策略做一次的处理
}

void TaiyiMdSpi::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspUserLogin ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return; 
    }
    LogDebug("OnRspUserLogin success, send SubscribeMarketData req");

    TaiyiModule *tym = TaiyiModule::GetInstance();
    int ret = tym->GetTaiyiMdApi()->SubscribeMarketData(tym->GetInstrumentID(), tym->GetInstrumentNum());
    if (!ret) {
        LogInfo("send SubscribeMarketData req success");
    } else {
        LogError("send SubscribeMarketData req failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnRspUserLogout(
    CThostFtdcUserLogoutField *pUserLogout,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspUserLogout ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return; 
    }
    LogDebug("OnRspUserLogout success, BrokerID: %s, UserID: %s", pUserLogout->BrokerID, pUserLogout->UserID);
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
        LogError("OnRspError ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    // TODO 异常处理
}

void TaiyiMdSpi::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspSubMarketData ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    // TODO 初始化行情内存缓存
    // TODO 起多线程分别处理不同的instrument每个线程处理一种Instrument;前期种类少时，可以试试单线程能否搞定
}

void TaiyiMdSpi::OnRspUnSubMarketData(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument, 
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID, 
	bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspUnSubMarketData ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    LogDebug("OnRspUnSubMarketData success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRspSubForQuoteRsp(
	CThostFtdcSpecificInstrumentField *pSpecificInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspSubForQuoteRsp ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    LogDebug("OnRspSubForQuoteRsp success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo,
    int nRequestID,
    bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LogError("OnRspSubForQuoteRsp ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
    LogDebug("OnRspSubForQuoteRsp success, InstrumentID: %s", pSpecificInstrument->InstrumentID);
}

void TaiyiMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    string instrumentID = pDepthMarketData->InstrumentID;
    TaiyiModule *tym = TaiyiModule::GetInstance();
    TaiyiInstrumentModule *tyim = tym->GetInstrumentModule(instrumentID);
    if (!tyim) {
        return;
    }
    tyim->SetMarketData(pDepthMarketData);
    
    // TODO 判断是否是当前交易日最后一个行情，如果是，触发行情持久化
}

void TaiyiMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {
    LogDebug("OnRtnForQuoteRsp TradingDay: %s ExchangeID: %s InstrumentID: %s ForQuoteSysID: %s",
        pForQuoteRsp->TradingDay, pForQuoteRsp->ExchangeID, pForQuoteRsp->InstrumentID, pForQuoteRsp->ForQuoteSysID);
}
