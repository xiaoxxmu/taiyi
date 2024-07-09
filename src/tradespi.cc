#include <cstring>
#include <cstdlib>

#include "tradespi.h"
#include "common.h"
#include "log.h"
#include "ctpapi.h"
#include "instrument.h"

extern char* g_pp_trade_instrument_id[];
extern uint32_t g_trade_instrument_cnt;
extern TThostFtdcBrokerIDType g_broker_id;
extern TThostFtdcInvestorIDType g_invester_id;
extern TThostFtdcPasswordType g_invester_pwd;
extern TThostFtdcAuthCodeType g_auth_code;
extern TThostFtdcAppIDType g_app_id;

extern CThostFtdcTraderApi* g_p_trader_api;

void TaiyiTraderSpi::OnFrontConnected() {
    LOG_INFO("OnFrontConnected success, start Login");

	BindCore(TRADE_THREAD_CORE_ID);

	CThostFtdcReqAuthenticateField req;
	memset(&req, 0, sizeof(CThostFtdcReqAuthenticateField));
	strcpy(req.BrokerID, g_broker_id);
	strcpy(req.UserID, g_invester_id);
	strcpy(req.AuthCode, g_auth_code);
	strcpy(req.AppID, g_app_id);
	int ret = g_p_trader_api->ReqAuthenticate(&req, 0);
    if (!ret) {
        LOG_ERROR("Send Authenticate req failed, ret %d", ret);
        return ;
    }
    LOG_INFO("Send Authenticate req success");
}

void TaiyiTraderSpi::OnFrontDisconnected(int nReason) {
	LOG_ERROR("OnFrontDisconnected nReason %d", nReason);
}

void TaiyiTraderSpi::OnHeartBeatWarning(int nTimeLapse) {
	LOG_ERROR("OnHeartBeatWarning nTimeLapse %d", nTimeLapse);
}

void TaiyiTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && pRspInfo->ErrorID != 0) {
		LOG_ERROR("OnRspAuthenticate failed, ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}
	LOG_INFO("OnRspAuthenticate success, start login");

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	strcpy(req.BrokerID, g_broker_id);
	strcpy(req.UserID, g_invester_id);
	strcpy(req.Password, g_invester_pwd);
	int ret = g_p_trader_api->ReqUserLogin(&req, 0);
    if (!ret) {
        LOG_ERROR("Send UserLogin req failed, ret %d", ret);
        return ;
    }
    LOG_INFO("Send UserLogin req success");
}

void TaiyiTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspUserLogin ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}


	SetTradeFrontId(pRspUserLogin->FrontID);
	SetSessionId(pRspUserLogin->SessionID);
	uint32_t max_order_ref = strtol(pRspUserLogin->MaxOrderRef, NULL, 10);
	SetOrderRef(max_order_ref);

	LOG_INFO("OnRspUserLogin Success, TradingDay %s LoginTime %s BrokerId  %s UserId %s front id %d session id %d MaxOrderRef %d",
		pRspUserLogin->TradingDay, pRspUserLogin->LoginTime, pRspUserLogin->BrokerID, pRspUserLogin->UserID, pRspUserLogin->FrontID, pRspUserLogin->SessionID, max_order_ref);

	// 发送投资者结算结果确认请求
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(CThostFtdcSettlementInfoConfirmField));
	strcpy(req.BrokerID, g_broker_id);
	strcpy(req.InvestorID, g_invester_id);
	int ret = g_p_trader_api->ReqSettlementInfoConfirm(&req, 0);
	if (!ret) {
		LOG_ERROR("ReqSettlementInfoConfirm ret %d", ret);
		return ;
	}
	LOG_INFO("ReqSettlementInfoConfirm send success");
}

void TaiyiTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspError ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}
}

void TaiyiTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspSettlementInfoConfirm ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}

	LOG_INFO("OnRspSettlementInfoConfirm BrokerID %s InvestorID %s ComfirmDate %s ConfirmTime %s SettlementID %d AccountID %s CurrencyID %s bIsLast %d",
		pSettlementInfoConfirm->BrokerID, pSettlementInfoConfirm->InvestorID, pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime,
		pSettlementInfoConfirm->SettlementID, pSettlementInfoConfirm->AccountID, pSettlementInfoConfirm->CurrencyID, bIsLast);

	if (!bIsLast) {
		return ;
	}

	// 查询交易合约信息
	for (uint32_t i = 0; i < g_trade_instrument_cnt; i++) {
		CThostFtdcQryInstrumentField req;
		memset(&req, 0, sizeof(CThostFtdcQryInstrumentField));
		strcpy(req.InstrumentID, g_pp_trade_instrument_id[i]);
		int ret = g_p_trader_api->ReqQryInstrument(&req, 0);
		if (ret) {
			LOG_ERROR("ReqQryInstrument %s ret %d", req.InstrumentID, ret);
		}
	}
}

void TaiyiTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspQryInstrument ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}

	if (!pInstrument) {
		return ;
	}

	// TODO LOG_INFO

	InstrumentHandle* handle = GetInstrumentHandleById(pInstrument->InstrumentID);
	if (!handle) {
		return ;
	}
	// TODO 记录合约的一些属性
	handle->SetExchangeId(pInstrument->ExchangeID);

	// 查询持仓信息
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(CThostFtdcQryInvestorPositionField));
	strcpy(req.BrokerID, g_broker_id);
	strcpy(req.InvestorID, g_invester_id);
	strcpy(req.InstrumentID, pInstrument->InstrumentID);
	strcpy(req.ExchangeID, pInstrument->ExchangeID);

	int ret = g_p_trader_api->ReqQryInvestorPosition(&req, 0);
	if (ret) {
		LOG_ERROR("ReqQryInvestorPosition %s ret %d", req.InstrumentID, ret);
	}
}



void TaiyiTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspQryInvestorPosition ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}

	if (!pInvestorPosition) {
		return ;
	}

	// TODO LOG_INFO

	InstrumentHandle* handle = GetInstrumentHandleById(pInvestorPosition->InstrumentID);
	if (!handle) {
		return ;
	}

	// TODO 更新持仓信息

	handle->SetTradeSwitchOn();
}


void TaiyiTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (!pInputOrder) {
		LOG_ERROR("OnRspOrderInsert invalid pInputOrder");
		return ;
	}
	LOG_ERROR("instrument(%s) order ref(%s) insert error(%d)",
		pInputOrder->InstrumentID, pInputOrder->OrderRef, pRspInfo->ErrorID);
	InstrumentHandle* handle = GetInstrumentHandleById(pInputOrder->InstrumentID);
	handle->OnErrRtnOrderInsert(pInputOrder);
}

void TaiyiTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
	if (!pRspInfo || !pInputOrder) {
		return;
	}
	LOG_ERROR("instrument(%s) order ref(%s) insert error(%d)",
		pInputOrder->InstrumentID, pInputOrder->OrderRef, pRspInfo->ErrorID);
	InstrumentHandle* handle = GetInstrumentHandleById(pInputOrder->InstrumentID);
	handle->OnErrRtnOrderInsert(pInputOrder);
}

void TaiyiTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
}

void TaiyiTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	if (!pOrder) { return; }
	InstrumentHandle* handle = GetInstrumentHandleById(pOrder->InstrumentID);
	handle->OnRtnOrder(pOrder);
}

void TaiyiTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	if (!pTrade) { return; }
	//InstrumentHandle* handle = GetInstrumentHandleById(pTrade->InstrumentID);
	//handle->OnRtnTrade(pTrade);
}
