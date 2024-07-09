#include <cstring>

#include "instrument.h"
#include "log.h"
#include "mdspi.h"
#include "common.h"

extern char* g_pp_md_instrument_id[];
extern uint32_t g_md_instrument_cnt;
extern TThostFtdcBrokerIDType g_broker_id;
extern TThostFtdcInvestorIDType g_invester_id;
extern TThostFtdcPasswordType g_invester_pwd;

extern CThostFtdcMdApi* g_p_md_user_api;

void TaiyiMdSpi::OnFrontConnected() {
    LOG_INFO("OnFrontConnected success, begin Login.");

    BindCore(MD_THREAD_CORE_ID);

    CThostFtdcReqUserLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
    strcpy(loginReq.BrokerID, g_broker_id);
    strcpy(loginReq.UserID, g_invester_id);
    strcpy(loginReq.Password, g_invester_pwd);

    int ret = g_p_md_user_api->ReqUserLogin(&loginReq, 0);
    if (!ret) {
        LOG_INFO("send ReqUserLogin success");
    } else {
        LOG_ERROR("send ReqUserLogin failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnFrontDisconnected(int nReason) {
    LOG_ERROR("OnFrontDisconnected nReason: 0x%x", nReason);
}

void TaiyiMdSpi::OnHeartBeatWarning(int nTimeLapse) {
    LOG_ERROR("OnHeartBeatWarning nTimeLapse: %d", nTimeLapse);
}

void TaiyiMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspUserLogin ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
    LOG_DEBUG("OnRspUserLogin success, send SubscribeMarketData req");

    int ret = g_p_md_user_api->SubscribeMarketData(g_pp_md_instrument_id, g_md_instrument_cnt);
    if (!ret) {
        LOG_INFO("send SubscribeMarketData req success");
    } else {
        LOG_ERROR("send SubscribeMarketData req failed, ret: %d", ret);
    }
}

void TaiyiMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspError ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    }
}

void TaiyiMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if (pRspInfo && pRspInfo->ErrorID != 0) {
        LOG_ERROR("OnRspSubMarketData ErrorID: %d ErrMsg: %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        return;
    }
}

void TaiyiMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    // TODO 判断行情是否合法
    if (!pDepthMarketData) { return ; }

    InstrumentHandle* handle = GetInstrumentHandleById(pDepthMarketData->InstrumentID);
    if (handle) {
        handle->SetMarketData(pDepthMarketData);
    }
}
