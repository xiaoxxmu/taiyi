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
#include "ctp_data.h"

CtpService::CtpService(Config* cfg) {
    _cfg = cfg;

    _pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    DBG_ASSERT(_pMdApi);
    _pMdSpi  = new TaiyiMdSpi(cfg, _pMdApi);
    DBG_ASSERT(_pMdSpi);
    _pMdApi->RegisterSpi(_pMdSpi);
    strcpy(_pMdFrontAddr, cfg->mdFrontAddr.c_str());
    _pMdApi->RegisterFront(_pMdFrontAddr);

    _pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(); // TODO FlowPath
    DBG_ASSERT(_pTraderApi);
    _pTraderSpi = new TaiyiTraderSpi(cfg, _pTraderApi);
    DBG_ASSERT(_pTraderSpi);
    _pTraderApi->RegisterSpi(_pTraderSpi);
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

Status CtpService::CancelOrder(OrderRefType ref) {
    CThostFtdcInputOrderActionField orderAction{};
    //memset(&orderAction, 0, sizeof(CThostFtdcInputOrderActionField));

    strcpy(orderAction.BrokerID, _cfg->brokerId.c_str());
    strcpy(orderAction.InvestorID, _cfg->investerId.c_str());
    strcpy(orderAction.UserID, _cfg->investerId.c_str());

    //orderAction.OrderRef

    //orderAction.FrontID = _cfg->traderFrontAddr;
    //orderAction.SessionID
    orderAction.ActionFlag = THOST_FTDC_AF_Delete;

    //orderAction.InstrumentID
    //orderAction.ExchangeID

    return _pTraderApi->ReqOrderAction(&orderAction, GetReqId());
}

OrderRefType CtpService::InsertOrder(std::string instrumentId, Order* order) {
    CThostFtdcInputOrderField ctpOrder{};
    memset(&ctpOrder, 0, sizeof(CThostFtdcInputOrderField));

    //BrokenID
    //InvestorID
    //InstrumentID
    //ExchangeID

    //ctpOrder.OrderRef = _ref;

    ctpOrder.LimitPrice = order->price;
    ctpOrder.VolumeTotalOriginal = order->volumeTotal;

    if (order->direction == ORDER_DIRECTION_BUY) {
        ctpOrder.Direction = THOST_FTDC_D_Buy;
    } else if (order->direction == ORDER_DIRECTION_SELL) {
        ctpOrder.Direction = THOST_FTDC_D_Sell;
    } else {
        return InvalidOrderRef;
    }

    if (order->combOffset == ORDER_COMB_OFFSET_OPEN) {
        ctpOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
    } else if (order->combOffset == ORDER_COMB_OFFSET_CLOSE) {
        ctpOrder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
    } else {
        return InvalidOrderRef;
    }

    if (order->price != 0.0) {
        ctpOrder.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    } else {
        //ctpOrder.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
        return InvalidOrderRef;
    }

    ctpOrder.TimeCondition = THOST_FTDC_TC_GFD; // 当日有效？
    ctpOrder.VolumeCondition = THOST_FTDC_VC_AV; // 任何数量？
    ctpOrder.MinVolume = 1;

    ctpOrder.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	ctpOrder.ContingentCondition = THOST_FTDC_CC_Immediately;
	ctpOrder.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	ctpOrder.IsAutoSuspend = 0;
	ctpOrder.UserForceClose = 0;

    int ret = _pTraderApi->ReqOrderInsert(&ctpOrder, GetReqId());
    if (!ret) {
        return InvalidOrderRef;
    }

    return _ref++;
}
