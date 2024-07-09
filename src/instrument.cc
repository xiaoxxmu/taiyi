#include <cstring>
#include <cstdlib>

#include "instrument.h"
#include "log.h"
#include "common.h"
#include "ctpapi.h"

extern char* g_pp_md_instrument_id[];
extern uint32_t g_md_instrument_cnt;
extern CThostFtdcTraderApi* g_p_trader_api;
extern TThostFtdcBrokerIDType g_broker_id;
extern TThostFtdcInvestorIDType g_invester_id;
extern TThostFtdcPasswordType g_invester_pwd;

InstrumentHandle** g_handles = nullptr;

InstrumentHandle** GetAllInstrumentHandle() {
    return g_handles;
}

InstrumentHandle* GetInstrumentHandleById(TThostFtdcInstrumentIDType instrumentId) {
    return g_handles[0];
}

InstrumentHandle* GetInstrumentHandleByIndex(uint32_t index) {
    return (index < g_md_instrument_cnt)  ? g_handles[index] : nullptr;
}

void StartPollingThread(LockFreeQueue* lfq) {
    std::thread* new_thread = new std::thread([lfq] {
        BindCore(lfq->GetCoreId());
        while (true) {
            LFQueueEvent event;
            if (lfq->Pop(event)) {
                InstrumentHandle* handle = GetInstrumentHandleByIndex(event.instrument_index);
                handle->DispatchEvent(event);
            }
            std::this_thread::yield();
        }
    });
    // TODO new_thread->join() and delete
}

void InitInstrumentHandle() {
    LockFreeQueue* common_lfq = new LockFreeQueue(COMMON_THREAD_CORE_ID);
    DBG_ASSERT(common_lfq);
    StartPollingThread(common_lfq);
    LockFreeQueue* trade_lfq = new LockFreeQueue(TRADE_THREAD_CORE_ID);
    DBG_ASSERT(trade_lfq);
    StartPollingThread(trade_lfq);

    g_handles = (InstrumentHandle**)malloc(sizeof(InstrumentHandle*)*g_md_instrument_cnt);
    DBG_ASSERT(g_handles);

    for (uint32_t i = 0; i < g_md_instrument_cnt; i++) {
        g_handles[i] = new InstrumentHandle(i);
        DBG_ASSERT(g_handles[i]);
        g_handles[i]->RegisterCommonLFQ(common_lfq);
        g_handles[i]->RegisterTradeLFQ(trade_lfq);
    }
}

InstrumentHandle::InstrumentHandle(uint32_t handle_index) {
    LOG_INFO("Create instrument handle, index(%d) id(%s)", handle_index, g_pp_md_instrument_id[handle_index]);

    _handle_index = handle_index;
    strcpy(_instrument_id, g_pp_md_instrument_id[handle_index]);

    _trade_open_for_md = true;
    _trade_read_for_md = false;
    _set_ready_by_trader = false;
    _trade_open_for_trade = false;

    _total_trade_cnt = 0;

    _market_data_size = MD_NUM_PER_HOUR * MAX_TRADE_WINDOW_HOUR;
    _market_data_cnt = 0;
    _pp_market_data = (CThostFtdcDepthMarketDataField**)malloc(sizeof(CThostFtdcDepthMarketDataField*)*_market_data_size);
    DBG_ASSERT(_pp_market_data);
    for (uint32_t i = 0; i < MAX_TRADE_WINDOW_HOUR; i++) {
        CThostFtdcDepthMarketDataField* tmp_md_buff = (CThostFtdcDepthMarketDataField*)malloc(sizeof(CThostFtdcDepthMarketDataField)*MD_NUM_PER_HOUR);
        DBG_ASSERT(tmp_md_buff);
        memset(tmp_md_buff, 0, sizeof(CThostFtdcDepthMarketDataField)*MD_NUM_PER_HOUR);
        for (uint32_t j = 0; j < MD_NUM_PER_HOUR; j++) {
            uint32_t md_index = i*MD_NUM_PER_HOUR + j;
            _pp_market_data[md_index] = &tmp_md_buff[j];
        }
    }

    _trade_signal_size = MAX_ORDER_NUM_PER_INSTRUMENT;
    _trade_signal_cnt = 0;
    _p_trade_signal = (TradeSignal*)malloc(sizeof(TradeSignal)*_trade_signal_size);
    DBG_ASSERT(_p_trade_signal);
    memset(_p_trade_signal, 0, sizeof(TradeSignal)*_trade_signal_size);

    _trade_order_index_size = MAX_ORDER_NUM_PER_INSTRUMENT;
    _trade_order_index_cnt = 0;
    _p_trade_order_index = (TradeOrderIndex*)malloc(sizeof(TradeOrderIndex)*_trade_order_index_size);
    DBG_ASSERT(_p_trade_order_index);
    memset(_p_trade_order_index, 0, sizeof(TradeOrderIndex)*_trade_order_index_size);

    _original_order_size = MAX_ORDER_NUM_PER_INSTRUMENT;
    _original_order_cnt = 0;
    _pp_original_order = (CThostFtdcInputOrderField**)malloc(sizeof(CThostFtdcInputOrderField*)*_original_order_size);
    DBG_ASSERT(_pp_original_order);
    CThostFtdcInputOrderField* tmp_ori_order_buff = (CThostFtdcInputOrderField*)malloc(sizeof(CThostFtdcInputOrderField)*_original_order_size);
    DBG_ASSERT(tmp_ori_order_buff);
    memset(tmp_ori_order_buff, 0, sizeof(CThostFtdcInputOrderField)*_original_order_size);
    // TODO 一些必要的初始化
    for (uint32_t i = 0; i < _original_order_size; i++) {
        _pp_original_order[i] = &tmp_ori_order_buff[i];
    }

    _trade_order_size = MAX_ORDER_NUM_PER_INSTRUMENT;
    _trade_order_cnt = 0;
    _pp_trade_order = (CThostFtdcOrderField**)malloc(sizeof(CThostFtdcOrderField*)*_trade_order_size);
    DBG_ASSERT(_pp_trade_order);
    CThostFtdcOrderField* tmp_trade_order_buff = (CThostFtdcOrderField*)malloc(sizeof(CThostFtdcOrderField)*_trade_order_size);
    DBG_ASSERT(tmp_trade_order_buff);
    memset(tmp_trade_order_buff, 0, sizeof(CThostFtdcOrderField)*_trade_order_size);
    for (uint32_t i = 0; i < _trade_order_size; i++) {
        _pp_trade_order[i] = &tmp_trade_order_buff[i];
    }

    _long_volume = 0;
    _short_volume = 0;

    _request_id = 0;
}

void InstrumentHandle::SetMarketData(CThostFtdcDepthMarketDataField* p_market_data) {
    if (_market_data_cnt >= _market_data_size) {
        return ;
    }
    memcpy(_pp_market_data[_market_data_cnt++], p_market_data, sizeof(CThostFtdcDepthMarketDataField));

    if (this->isTradeReadyForMdThread()) {
        this->Strategy();
    }

    // TODO 加入到持久化队列
}

void InstrumentHandle::Strategy() {
    TradeSignal ts;
    CThostFtdcDepthMarketDataField* last_market_data = _pp_market_data[_market_data_cnt-1];
    ts.price = last_market_data->LastPrice;
    ts.volume = 1;
    ts.signal = _market_data_cnt % 2; // 奇买偶平
    if (this->SetTradeSignal(ts)) {
        this->SendTradeEvent();
    }
}

void InstrumentHandle::DispatchEvent(const LFQueueEvent& event) {
    switch(event.event_type) {
        case LFQ_EVENT_TRADE_SIGNAL:
            if (this->isTradeReadyForTradeThread()) {
                this->TradeSignalEvent(event);
            }
            break;
    }
}

void InstrumentHandle::TradeSignalEvent(const LFQueueEvent& event) {
    TradeSignal* ts = this->GetTradeSignal(event.event_index);

    if (_total_trade_cnt + 1 >= MAX_ORDER_NUM_PER_INSTRUMENT) { // 合约交易次数达到限制时，只处理平仓信号
        if (ts->signal == 0) {
            this->ClosePosition(ts);
        }
        return ;
    }

    switch (ts->signal) {
        case -1:
            this->GoingShort(ts);
            break;
        case 0:
            this->ClosePosition(ts);
            break;
        case 1:
            this->GoingLong(ts);
            break;
        default:
            LOG_ERROR("invalid signal %d", ts->signal);
    }
}

void InstrumentHandle::GoingShort(TradeSignal* ts) {
    bool need_sell_open = true;
    bool need_sell_close = true;

    for (auto it = _unfinished_order_index_map.begin(); it != _unfinished_order_index_map.end(); it++) {
        CThostFtdcInputOrderField* order = GetOriginalOrderByIndex(it->second->original_order_index);
        if (order->Direction == THOST_FTDC_D_Buy) {
            this->CancelOrder(order);
        } else if (order->Direction == THOST_FTDC_D_Sell) {
            if (order->CombOffsetFlag[0] = THOST_FTDC_OF_Close) {
                need_sell_close = false;
            } else {
                need_sell_open = false;
            }
            // TODO 根据价格进行改单
            // this->ModifyOrder();
        }
    }

    if (need_sell_close && _long_volume > 0) {
        this->InsertOrder(THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, ts->price, _long_volume);
    }

    if (need_sell_open) {
        this->InsertOrder(THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, ts->price, ts->volume);
    }
}

void InstrumentHandle::GoingLong(TradeSignal* ts) {
    bool need_buy_open = true;
    bool need_buy_close = true;

    for (auto it = _unfinished_order_index_map.begin(); it != _unfinished_order_index_map.end(); it++) {
        CThostFtdcInputOrderField* order = GetOriginalOrderByIndex(it->second->original_order_index);
        if (order->Direction == THOST_FTDC_D_Sell) {
            this->CancelOrder(order);
        } else if (order->Direction == THOST_FTDC_D_Buy) {
            if (order->CombOffsetFlag[0] = THOST_FTDC_OF_Close) {
                need_buy_close = false;
            } else {
                need_buy_open = false;
            }
            // TODO 根据价格进行改单
            // this->ModifyOrder();
        }
    }

    if (need_buy_close && _short_volume > 0) {
        this->InsertOrder(THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, ts->price, _short_volume);
    }

    if (need_buy_open) {
        this->InsertOrder(THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, ts->price, ts->volume);
    }
}

void InstrumentHandle::ClosePosition(TradeSignal* ts) {
    for (auto it = _unfinished_order_index_map.begin(); it != _unfinished_order_index_map.end(); it++) {
        CThostFtdcInputOrderField* order = GetOriginalOrderByIndex(it->second->original_order_index);
        this->CancelOrder(order);
    }

    if (_long_volume > 0) {
        this->InsertOrder(THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, ts->price, _long_volume);
    }

    if (_short_volume > 0) {
        this->InsertOrder(THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, ts->price, _short_volume);
    }
}

void InstrumentHandle::InsertOrder(TThostFtdcDirectionType direction, char comb_off, double price, uint32_t volume) {
    CThostFtdcInputOrderField* order = _pp_original_order[_original_order_cnt];

    uint32_t order_ref = GetOrderRef();

    strcpy(order->BrokerID, g_broker_id);
	strcpy(order->InvestorID, g_invester_id);
	strcpy(order->InstrumentID, _instrument_id);
	strcpy(order->ExchangeID, _exchange_id);

    sprintf(order->OrderRef, "%u", order_ref);
    order->Direction = direction;
    order->CombOffsetFlag[0] = comb_off;
    order->OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    order->LimitPrice = price;
    order->VolumeTotalOriginal = volume;

    order->TimeCondition = THOST_FTDC_TC_GFD; // 当日有效？
    order->VolumeCondition = THOST_FTDC_VC_AV; // 任何数量？
    order->MinVolume = 1;

    order->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    order->ContingentCondition = THOST_FTDC_CC_Immediately;
    order->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    order->IsAutoSuspend = 0;
    order->UserForceClose = 0;

    int ret = g_p_trader_api->ReqOrderInsert(order, _request_id++);
    if (ret) {
        return ;
    }

    _total_trade_cnt++;

    TradeOrderIndex* order_index = &_p_trade_order_index[_trade_order_index_cnt++];
    order_index->original_order_index = _original_order_cnt;
    _original_order_cnt++;
    CThostFtdcOrderField* trade_order = _pp_trade_order[_trade_order_index_cnt];
    order_index->trade_order_index = _trade_order_index_cnt;
    _trade_order_index_cnt++;
    _unfinished_order_index_map.insert(std::pair<uint32_t, TradeOrderIndex*>(order_ref, order_index));
}

void InstrumentHandle::CancelOrder(CThostFtdcInputOrderField* order) {
    CThostFtdcInputOrderActionField order_action;
    memset(&order_action, 0, sizeof(CThostFtdcInputOrderActionField));

    strcpy(order_action.BrokerID, g_broker_id);
    strcpy(order_action.InvestorID, g_invester_id);
    strcpy(order_action.UserID, g_invester_id);
    strcpy(order_action.OrderRef, order->OrderRef);
    order_action.FrontID = GetTradeFrontId();
	order_action.SessionID = GetSessionId();
	order_action.ActionFlag = THOST_FTDC_AF_Delete;

    int ret = g_p_trader_api->ReqOrderAction(&order_action, _request_id++);
    if (ret) {
        return ;
    }
    _total_trade_cnt++;
}

void InstrumentHandle::OnErrRtnOrderInsert(CThostFtdcInputOrderField *p_input_order) {
    uint32_t order_ref = strtol(p_input_order->OrderRef, NULL, 10);
    std::map<uint32_t, TradeOrderIndex*>::iterator iter;
    iter = _unfinished_order_index_map.find(order_ref);
    if (iter == _unfinished_order_index_map.end()) {
        return ;
    }
    _unfinished_order_index_map.erase(iter);
}

void InstrumentHandle::OnRtnOrder(CThostFtdcOrderField *p_order) {
    uint32_t order_ref = strtol(p_order->OrderRef, NULL, 10);
    std::map<uint32_t, TradeOrderIndex*>::iterator iter;
    iter = _unfinished_order_index_map.find(order_ref);
    if (iter == _unfinished_order_index_map.end()) {
        return ;
    }

    // TODO 当前volume为1，暂时不考虑复杂的计算
    memcpy(_pp_trade_order[iter->second->trade_order_index], p_order, sizeof(CThostFtdcOrderField));

    if (p_order->OrderStatus == THOST_FTDC_OST_Canceled) {
        _unfinished_order_index_map.erase(iter);
        return ;
    }

    if (p_order->OrderStatus == THOST_FTDC_OST_AllTraded) {
        if (p_order->Direction == THOST_FTDC_D_Buy) {
            if (p_order->CombOffsetFlag[0] == THOST_FTDC_OF_Open) {
                _long_volume += p_order->VolumeTotal;
            } else if (p_order->CombOffsetFlag[0] == THOST_FTDC_OF_Close) {
                _short_volume = (_short_volume > p_order->VolumeTotal) ? _short_volume - p_order->VolumeTotal : 0;
            }
        } else if (p_order->Direction == THOST_FTDC_D_Sell) {
            if (p_order->CombOffsetFlag[0] == THOST_FTDC_OF_Open) {
                _short_volume += p_order->VolumeTotal;
            } else if (p_order->CombOffsetFlag[0] == THOST_FTDC_OF_Close) {
                _long_volume = (_long_volume > p_order->VolumeTotal) ? _long_volume - p_order->VolumeTotal : 0;
            }
        }
        _unfinished_order_index_map.erase(iter);
        return ;
    }
}
