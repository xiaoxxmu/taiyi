/*
 * instrument_trade.cc
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#include "instrument_trade.h"

#include "log.h"
#include "message.h"
#include "protocol.h"

InstrumentTradeModule::InstrumentTradeModule(
    uint32_t moduleId, Container* pContainer,
    ICtpService* pCtpService) : Module(moduleId, pContainer) {
    _pCtpService = pCtpService;

    // TODO
}

Status InstrumentTradeModule::Dispatch(TaiyiMessage* msg) {
    switch (msg->cmd) {
        case PushTradeSignalReqCmd:
            return HandlePushTradeSignalReq(msg);
        case OrderInsertRspErrorCmd:
            return HandleOrderInsertErrorRsp(msg);
        case OrderTradedRspCmd:
            return HandleOrderTradedRsp(msg);
    }
    return StatusOK;
}

Status InstrumentTradeModule::HandlePushTradeSignalReq(TaiyiMessage* msg) {
    InstrumentTradeSignalReq* req = (InstrumentTradeSignalReq*)msg->data[0];
    TradeSignal signal = {
        .signal=req->signal,
        .price=req->price,
        .volume = req->volume};
    AddTradeSignal(req->curMdNum-1, signal); // TODO 判断信号是否合法，通过idx时间戳等信息来判断
    return HandleTradeSignal();
}

Status InstrumentTradeModule::HandleOrderInsertErrorRsp(TaiyiMessage* msg) {
    OrderInsertErrorRsp* rsp = (OrderInsertErrorRsp*)msg->data[0];

    std::map<OrderRefType, Order*>::iterator iter;
    iter = _unfinishedOrder.find(rsp->ref);
    Order* order = iter->second;
    order->tradeStatus = ORDER_TRADE_STATUS_ERROR_INSERT;
    _unfinishedOrder.erase(order->ref);

    return StatusOK;
}

Status InstrumentTradeModule::HandleOrderTradedRsp(TaiyiMessage* msg) {
    int curTraded = 0;
    OrderTradedRsp* rsp = (OrderTradedRsp*)msg->data[0];

    std::map<OrderRefType, Order*>::iterator iter;
    iter = _unfinishedOrder.find(rsp->ref);
    Order* order = iter->second;

    order->volumeTotal = rsp->volumeTotal;
    order->tradeStatus = rsp->tradeStatus;
    if (rsp->volumeTraded > order->volumeTraded) {
        curTraded = rsp->volumeTraded - order->volumeTraded;
    }
    order->volumeTraded = rsp->volumeTraded;

    if (order->combOffset == ORDER_COMB_OFFSET_CLOSE) {
        if (order->direction == ORDER_DIRECTION_BUY) {
            _position.shortVolume -= curTraded;
        } else if (order->direction == ORDER_DIRECTION_SELL) {
            _position.longVolume -= curTraded;
        }
    } else if (order->combOffset == ORDER_COMB_OFFSET_OPEN) {
        if (order->direction == ORDER_DIRECTION_BUY) {
            _position.longVolume += curTraded;
        } else if (order->direction == ORDER_DIRECTION_SELL) {
            _position.shortVolume += curTraded;
        }
    }

    // TODO 这里计算一下收益

    if (order->tradeStatus == ORDER_TRADE_STATUS_ALL_TRADED || order->tradeStatus == ORDER_TRADE_STATUS_CANCELED) {
        _unfinishedOrder.erase(order->ref);
    }

    return StatusOK;
}

Status InstrumentTradeModule::HandleTradeSignal() {
    TradeInfo* pInfo = GetLastTradeInfo();
    switch (pInfo->signal.signal) {
        case -1:
            return GoingShort();
        case 0:
            return ClosePosition();
        case 1:
            return GoingLong();
        default:
            LOG_ERROR("Invalid signal %d", pInfo->signal.signal);
    }
    return StatusError;
}

// 做空信号，撤多单，平多仓，下空单
Status InstrumentTradeModule::GoingShort() {
    TradeInfo* pInfo = GetLastTradeInfo();
    if (!pInfo) {
        return StatusError;
    }

    int shortOrderVolume = 0;
    int sellCloseOrderVolume = 0; // 平多单数量
    for (auto it = _unfinishedOrder.begin(); it != _unfinishedOrder.end(); it++) {
        Order* order = it->second;
        if (order->direction == ORDER_DIRECTION_BUY) { // 挂单为多单，则撤单
            CancelOrder(order->ref);
        } else if (order->direction == ORDER_DIRECTION_SELL) {
            // 挂单为空单，这里先不做处理
            // TODO 根据价格判断是否需要撤单, 如果撤单，则在算入最后需要新下单的数量
            shortOrderVolume += order->volumeTotal - order->volumeTraded;
            if (order->combOffset == ORDER_COMB_OFFSET_CLOSE) {
                sellCloseOrderVolume += order->volumeTotal - order->volumeTraded;
            }
        } else {
            LOG_ERROR("invalid order direction %d", order->direction);
        }
    }

    if (_position.longVolume > sellCloseOrderVolume) {
        int leftLongVolume = _position.longVolume - sellCloseOrderVolume;
        InsertOrder(ORDER_DIRECTION_SELL, ORDER_COMB_OFFSET_CLOSE, leftLongVolume);
    }

    if (pInfo->signal.volume > shortOrderVolume + _position.shortVolume) { // 挂单和持仓数量不足，补仓
        int deltaShortVolume = pInfo->signal.volume - shortOrderVolume - _position.shortVolume;
        InsertOrder(ORDER_DIRECTION_SELL, ORDER_COMB_OFFSET_OPEN, deltaShortVolume);
    }

    return StatusOK;
}

// 做多信号，撤空单，平空仓，下多单
Status InstrumentTradeModule::GoingLong() {
    TradeInfo* pInfo = GetLastTradeInfo();
    if (!pInfo) {
        return StatusError;
    }

    int longOrderVolume = 0;
    int buyCloseOrderVolume = 0; // 平空单数量
    for (auto it = _unfinishedOrder.begin(); it != _unfinishedOrder.end(); it++) {
        Order* order = it->second;
        if (order->direction == ORDER_DIRECTION_BUY) {
            // 挂单为多单，这里先不做处理；
            // TODO 根据价格判断是否需要撤单, 如果撤单，则在算入最后需要新下单的数量
            longOrderVolume += order->volumeTotal - order->volumeTraded;
            if (order->combOffset == ORDER_COMB_OFFSET_CLOSE) {
                buyCloseOrderVolume +=  order->volumeTotal - order->volumeTraded;
            }
        } else if (order->direction == ORDER_DIRECTION_SELL) {
            CancelOrder(order->ref);
        } else {
            LOG_ERROR("invalid order direction %d", order->direction);
        }
    }

    if (_position.shortVolume > buyCloseOrderVolume) {
        int leftShortVolume = _position.shortVolume - buyCloseOrderVolume;
        InsertOrder(ORDER_DIRECTION_BUY, ORDER_COMB_OFFSET_CLOSE, leftShortVolume);
    }

    if (pInfo->signal.volume > longOrderVolume + _position.longVolume) {
        int deltaLongVolume = pInfo->signal.volume - longOrderVolume - _position.longVolume;
        InsertOrder(ORDER_DIRECTION_BUY, ORDER_COMB_OFFSET_OPEN, deltaLongVolume);
    }

    return StatusOK;
}

// 平仓信号，撤单，平仓
Status InstrumentTradeModule::ClosePosition() {
    TradeInfo* pInfo = GetLastTradeInfo();
    if (!pInfo) {
        return StatusError;
    }

    for (auto it = _unfinishedOrder.begin(); it != _unfinishedOrder.end(); it++) {
        Order* order = it->second;
        CancelOrder(order->ref);
    }

    if (_position.shortVolume > 0) {
        InsertOrder(ORDER_DIRECTION_BUY, ORDER_COMB_OFFSET_CLOSE, _position.shortVolume);
    }

    if (_position.longVolume > 0) {
        InsertOrder(ORDER_DIRECTION_SELL, ORDER_COMB_OFFSET_CLOSE, _position.longVolume);
    }

    return StatusOK;
}

Status InstrumentTradeModule::InsertOrder(OrderDirectionType direction, OrderCombOffsetType combOffset, int volume) {
    TradeInfo* pInfo = GetLastTradeInfo();
    DBG_ASSERT(pInfo);
    Order* order = (Order*)_pOrderPool->Zalloc();
    DBG_ASSERT(order);

    order->direction = direction;
    order->combOffset = combOffset;
    order->volumeTotal = volume;
    order->price = pInfo->signal.price;
    order->tradeStatus = ORDER_TRADE_STATUS_INIT;
    order->volumeTraded = 0;

    order->ref = _pCtpService->InsertOrder(_instrumentId, order);
    if (!order->ref) {
        return StatusError;
    }

    pInfo->orders.push_back(order);
    _unfinishedOrder.insert(std::pair<OrderRefType, Order*>(order->ref, order));

    return StatusOK;
}

Status InstrumentTradeModule::CancelOrder(OrderRefType ref) {
    TradeInfo* pInfo = GetLastTradeInfo();
    DBG_ASSERT(pInfo);

    OrderAction* action = (OrderAction*)_pOrderActionPool->Zalloc();
    DBG_ASSERT(action);
    action->ref = ref;
    Status ret = _pCtpService->CancelOrder(ref);
    if (ret) {
        LOG_ERROR("Cancel order ref(%u) ret(%d)", ref, ret);
        return ret;
    }

    pInfo->actions.push_back(action);

    return StatusOK;
}
