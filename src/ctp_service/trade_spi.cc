/*
 * trade_spi.cc
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#include <string>

#include "trade_spi.h"

#include "protocol.h"
#include "log.h"
#include "proc.h"
#include "module.h"

void TaiyiTraderSpi::OnFrontConnected() {
    LOG_INFO("OnFrontConnected success, start Login");

	CThostFtdcReqAuthenticateField req;
	memset(&req, 0, sizeof(CThostFtdcReqAuthenticateField));
	strcpy(req.BrokerID, _brokerId);
	strcpy(req.UserID, _investerId);
	strcpy(req.AuthCode, _authCode);
	strcpy(req.AppID, _appId);
	int ret = _pTraderApi->ReqAuthenticate(&req, 0);
    if (!ret) {
        LOG_ERROR("Send Authenticate req failed, ret %d", ret);
        return ; // TODO 异常处理
    }
    LOG_INFO("Send Authenticate req success");
}

void TaiyiTraderSpi::OnFrontDisconnected(int nReason) {
	LOG_ERROR("OnFrontDisconnected nReason %d", nReason);
}

void TaiyiTraderSpi::OnHeartBeatWarning(int nTimeLapse) {
	LOG_ERROR("OnHeartBeatWarning nTimeLapse %d", nTimeLapse);
}

void TaiyiTraderSpi::OnRspAuthenticate(
	CThostFtdcRspAuthenticateField* pRspAuthenticateField,
	CThostFtdcRspInfoField* pRspInfo,
	int nRequestID,
	bool bIsLast) {
	if (pRspInfo && pRspInfo->ErrorID != 0) {
		LOG_ERROR("OnRspAuthenticate failed, ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ;
	}
	LOG_INFO("OnRspAuthenticate success, start login");

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	strcpy(req.BrokerID, _brokerId);
	strcpy(req.UserID, _investerId);
	strcpy(req.Password, _investerPwd);
	int ret = _pTraderApi->ReqUserLogin(&req, 0);
    if (!ret) {
        LOG_ERROR("Send UserLogin req failed, ret %d", ret);
        return ; // TODO 异常处理
    }
    LOG_INFO("Send UserLogin req success");
}

void TaiyiTraderSpi::OnRspUserLogin(
	CThostFtdcRspUserLoginField *pRspUserLogin,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspUserLogin ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ; // TODO 异常处理
	}
	LOG_INFO("OnRspUserLogin Success, TradingDay %s LoginTime %s BrokerId  %s UserId %s",
		pRspUserLogin->TradingDay, pRspUserLogin->LoginTime, pRspUserLogin->BrokerID, pRspUserLogin->UserID);

	_tradeFrontId = pRspUserLogin->FrontID;
	_sessionId = pRspUserLogin->SessionID;
	strcpy(_orderRef, pRspUserLogin->MaxOrderRef);

	// TODO 查询一下各种账户信息、合约的持仓信息（上一个周期由于故障等原因尚未平仓的信息等）
}

void TaiyiTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
	if (pRspInfo && (pRspInfo->ErrorID != 0)) {
		LOG_ERROR("OnRspError ErrorID %d ErrorMsg %s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		return ; // TODO 异常处理
	}
}

void TaiyiTraderSpi::OnRspUserLogout(
	CThostFtdcUserLogoutField *pUserLogout,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	// TODO 账号切换？
}

void TaiyiTraderSpi::OnRspSettlementInfoConfirm(
	CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	//TODO
}

void TaiyiTraderSpi::OnRspQryInstrument(
	CThostFtdcInstrumentField *pInstrument,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	// TODO
}

void TaiyiTraderSpi::OnRspQryTradingAccount(
	CThostFtdcTradingAccountField *pTradingAccount,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	// TODO
}

void TaiyiTraderSpi::OnRspQryInvestorPosition(
	CThostFtdcInvestorPositionField *pInvestorPosition,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	// TODO
}

void TaiyiTraderSpi::OnRspOrderInsert(
	CThostFtdcInputOrderField *pInputOrder,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	if (!pInputOrder) {
		LOG_ERROR("OnRspOrderInsert invalid pInputOrder");
		return ;
	}
	OnErrRtnOrderInsert(pInputOrder, pRspInfo);
}

void TaiyiTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
	if (!pRspInfo || !pInputOrder) {
		return;
	}

	std::string instrumentId = pInputOrder->InstrumentID;
	InstrumentLocation* location = TaiyiMain()->GetInstrumentLocation(instrumentId);
    if (!location) {
        LOG_ERROR("InstrumentId %s NOT FOUND", instrumentId.c_str());
        return ;
    }

	TaiyiMessage* msg = (TaiyiMessage*)_pMsgPool->Zalloc();
    DBG_ASSERT(msg);
	msg->cmd = OrderInsertRspErrorCmd;
	msg->srcContainerId = _containerId;
    msg->srcModuleId = _moduleId;
    msg->dstContainerId = location->tradeContainer->GetContainerId();
    msg->dstModuleId = location->tradeModule->GetModuleId();

	OrderInsertErrorRsp* rsp = (OrderInsertErrorRsp*)_pOrderInsertRspPool->Zalloc();
	DBG_ASSERT(rsp);
	rsp->retcode = pRspInfo->ErrorID;
	rsp->ref = strtol(pInputOrder->OrderRef, NULL, 10);
	msg->data[0] = rsp;

	if (location->tradeContainer->Push(msg)) {
        // TODO 异常或重试队列？
    }
}

void TaiyiTraderSpi::OnRspOrderAction(
	CThostFtdcInputOrderActionField *pInputOrderAction,
	CThostFtdcRspInfoField *pRspInfo,
	int nRequestID,
	bool bIsLast) {
	//TODO
}

void TaiyiTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) {
	if (!pOrder) {return;}

	std::string instrumentId = pOrder->InstrumentID;
	InstrumentLocation* location = TaiyiMain()->GetInstrumentLocation(instrumentId);
    if (!location) {
        LOG_ERROR("InstrumentId %s NOT FOUND", instrumentId.c_str());
        return ;
    }

	TaiyiMessage* msg = (TaiyiMessage*)_pMsgPool->Zalloc();
    DBG_ASSERT(msg);
	msg->cmd = OrderTradedRspCmd;
	msg->srcContainerId = _containerId;
    msg->srcModuleId = _moduleId;
    msg->dstContainerId = location->tradeContainer->GetContainerId();
    msg->dstModuleId = location->tradeModule->GetModuleId();

	OrderTradedRsp* rsp = (OrderTradedRsp*)_pOrderTradedRspPool->Zalloc();
	DBG_ASSERT(rsp);

	rsp->ref = strtol(pOrder->OrderRef, NULL, 10);

	rsp->volumeTotal = pOrder->VolumeTotal;
	rsp->price = pOrder->LimitPrice;
	rsp->volumeTraded = pOrder->VolumeTraded;

	if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled) {
		rsp->tradeStatus = ORDER_TRADE_STATUS_CANCELED;
	} else if (pOrder->VolumeTotal == pOrder->VolumeTraded) {
		rsp->tradeStatus = ORDER_TRADE_STATUS_ALL_TRADED;
	} else {
		rsp->tradeStatus = ORDER_TRADE_STATUS_PART_TRADED;
	}

	msg->data[0] = rsp;
	if (location->tradeContainer->Push(msg)) {
        // TODO 异常或重试队列？
    }
}

void TaiyiTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) {
	// TODO
}

/////////////////////////////////////////////////////////////////////////////////////

TaiyiTraderSpi::TaiyiTraderSpi(Config* cfg, CThostFtdcTraderApi* pTraderApi) {
	 _pTraderApi = pTraderApi;
    _containerId = 0;
    _moduleId = 0;

    strcpy(_brokerId, cfg->brokerId.c_str());
    strcpy(_investerId, cfg->investerId.c_str());
    strcpy(_investerPwd, cfg->investerPwd.c_str());
	strcpy(_authCode, cfg->authCode.c_str());
    strcpy(_appId, cfg->appId.c_str());

    uint32_t memPoolSize = cfg->instruments.size() * MD_NUM_PER_INSTRUMENT;
    _pMsgPool = new TaiyiMemPool(memPoolSize, sizeof(TaiyiMessage));
    DBG_ASSERT(_pMsgPool);
}
