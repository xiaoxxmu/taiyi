#pragma once

#include <mutex>
#include <thread>
#include <cstring>
#include <map>

#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcUserApiDataType.h"

#include "common.h"
#include "lfqueue.h"

struct TradeSignal {
    int32_t signal;
    int32_t volume;
    double price;
};

struct TradeOrderIndex {
    int32_t original_order_index;
    int32_t trade_order_index;
};

class InstrumentHandle {
  public:
    InstrumentHandle(uint32_t handle_index);
    ~InstrumentHandle() {
        if (_pp_market_data) {
            for (uint32_t i = 0; i < MAX_TRADE_WINDOW_HOUR; i++) {
                if (_pp_market_data[i*MD_NUM_PER_HOUR]) {
                    free(_pp_market_data[i*MD_NUM_PER_HOUR]);
                    _pp_market_data[i*MD_NUM_PER_HOUR] = nullptr;
                }
            }
            free(_pp_market_data);
            _pp_market_data = nullptr;
        }
        if (_p_trade_signal) {
            free(_p_trade_signal);
            _p_trade_signal = nullptr;
        }
        if (_p_trade_order_index) {
            free(_p_trade_order_index);
            _p_trade_order_index = nullptr;
        }
        if (_pp_original_order) {
            if (_pp_original_order[0]) {
                free(_pp_original_order[0]);
                _pp_original_order[0] = nullptr;
            }
            free(_pp_original_order);
            _pp_original_order = nullptr;
        }
        if (_pp_trade_order) {
            if (_pp_trade_order[0]) {
                free(_pp_trade_order[0]);
                _pp_trade_order[0] = nullptr;
            }
            free(_pp_trade_order);
            _pp_trade_order = nullptr;
        }
    }

    void DispatchEvent(const LFQueueEvent& event);

    void TradeSignalEvent(const LFQueueEvent& event);
    void GoingLong(TradeSignal* ts);
    void GoingShort(TradeSignal* ts);
    void ClosePosition(TradeSignal* ts);

    void InsertOrder(TThostFtdcDirectionType direction, char comb_off, double price, uint32_t volume);
    void CancelOrder(CThostFtdcInputOrderField* order);

    void OnErrRtnOrderInsert(CThostFtdcInputOrderField *p_input_order);
    void OnRtnOrder(CThostFtdcOrderField *p_order);

    uint32_t _handle_index;
    TThostFtdcInstrumentIDType _instrument_id;
    TThostFtdcExchangeIDType _exchange_id;

    void SetExchangeId(TThostFtdcExchangeIDType exchange_id) { strcpy(_exchange_id, exchange_id); }

    // 一些只能在行情线程使用的开关，访问时不需要加锁
    bool _trade_open_for_md; // 是否开启交易，如果不开启，则只收集行情
    bool _trade_read_for_md; // 交易是否已经准备好，只需要初始化一次

    // 在交易线程设置，在行情线程使用的开关，需要加锁
    bool _set_ready_by_trader;
    std::mutex _set_ready_by_trader_mutex;

    bool _trade_open_for_trade; // 交易线程的交易开关
    uint32_t _total_trade_cnt; // 总共交易的次数，包括下单和撤单

    void SetTradeSwitchOn() {
        _trade_open_for_trade = true;
        std::lock_guard<std::mutex> lk(_set_ready_by_trader_mutex);
        _set_ready_by_trader = true;
    }

    bool isTradeReadyForMdThread() {
        if (!_trade_open_for_md) {
            return false;
        }
        if (_trade_read_for_md) {
            return true;
        }
        std::lock_guard<std::mutex> lk(_set_ready_by_trader_mutex);
        if (_set_ready_by_trader) {
            _trade_read_for_md = true;
            return true;
        }
        return false;
    }

    bool isTradeReadyForTradeThread() {
        return _trade_open_for_trade;
    }

    // 行情信息，只能够在行情线程上修改，其他线程上只读，且只读的偏移小于cnt，以确保线程安全
    CThostFtdcDepthMarketDataField** _pp_market_data;
    uint32_t _market_data_size;
    uint32_t _market_data_cnt;

    void SetMarketData(CThostFtdcDepthMarketDataField* p_market_data);
    CThostFtdcDepthMarketDataField* GetMarketData(uint32_t index) {
        return index < _market_data_size ? _pp_market_data[index] : nullptr;
    }

    void Strategy();

    // 交易信息，只能够在行情线程上修改，其他线程上只读，且只读的偏移小于cnt，以确保线程安全
    TradeSignal* _p_trade_signal;
    uint32_t _trade_signal_size;
    uint32_t _trade_signal_cnt;

    bool SetTradeSignal(const TradeSignal& ts) {
        if (_trade_signal_cnt < _trade_signal_size) {
            memcpy(&_p_trade_signal[_trade_signal_cnt], &ts, sizeof(TradeSignal));
            _trade_signal_cnt++;
            return true;
        } else {
            return false;
        }
    }
    TradeSignal* GetTradeSignal(uint32_t index) {
        return index < _trade_signal_size ? &_p_trade_signal[index] : nullptr;
    }

    LockFreeQueue* _p_trade_event_lfq; // 交易信号事件队列
    LockFreeQueue* _p_common_event_lfq; // 公共事件信号队列

    void RegisterCommonLFQ(LockFreeQueue* lfq) { _p_common_event_lfq = lfq; }
    void RegisterTradeLFQ(LockFreeQueue* lfq) { _p_trade_event_lfq = lfq; }

    void SendTradeEvent() {
        LFQueueEvent event = {
            .instrument_index = _handle_index,
            .event_type = LFQ_EVENT_TRADE_SIGNAL,
            .event_index = _trade_signal_cnt - 1
        };
        while (!_p_trade_event_lfq->Push(event)) {
            std::this_thread::yield();
        }
    }

    TradeOrderIndex* _p_trade_order_index;
    uint32_t _trade_order_index_size;
    uint32_t _trade_order_index_cnt;
    CThostFtdcInputOrderField** _pp_original_order;
    uint32_t _original_order_size;
    uint32_t _original_order_cnt;
    CThostFtdcOrderField** _pp_trade_order;
    uint32_t _trade_order_size;
    uint32_t _trade_order_cnt;

    CThostFtdcInputOrderField* GetOriginalOrderByIndex(uint32_t index) {
        return index < _original_order_size ? _pp_original_order[index] : nullptr;
    }

    std::map<uint32_t, TradeOrderIndex*> _unfinished_order_index_map;



    // 持仓信息
    uint32_t _long_volume;
    uint32_t _short_volume;

    int _request_id;


};

InstrumentHandle** GetAllInstrumentHandle();
InstrumentHandle* GetInstrumentHandleById(TThostFtdcInstrumentIDType instrumentId);
InstrumentHandle* GetInstrumentHandleByIndex(uint32_t index);
void InitInstrumentHandle();
