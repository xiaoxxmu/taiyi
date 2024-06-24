/*
 * ctp_data.h
 *
 * Created on: 20240624
 *      Author xiaoxxmu
 *
 */

#pragma once

typedef char TimeType[9];
typedef uint32_t OrderRefType;

const static OrderRefType InvalidOrderRef = 0;

enum OrderDirectionType {
    ORDER_DIRECTION_BUY,
    ORDER_DIRECTION_SELL,
};

enum OrderCombOffsetType {
    ORDER_COMB_OFFSET_OPEN,
    ORDER_COMB_OFFSET_CLOSE,
};

enum OrderTradeStatus {
    StatusInit,
    AllTraded,
    PartTraded,
    Canceled,
};

// 深度行情
struct MarketData
{
	TimeType UpdateTime;
	int	UpdateMillisec;

    double LastPrice; // 最新价
    int Volume; // 数量

    double PreSettlementPrice; // 上次结算价
    double SettlementPrice; // 本次结算价

	double UpperLimitPrice; // 涨停板价
	double LowerLimitPrice; // 跌停板价

    double BidPrice[5];
    int BidVolume[5];
    double AskPrice[5];
    int AskVolume[5];

    double AveragePrice;
};

struct TradeSignal {
    int signal;
    double price;
    int volume;
};

// 持仓
struct Position {
    int longVolume;
    int shortVolume;
};

// 订单
struct Order {
    OrderRefType ref;
    OrderDirectionType direction;
    OrderCombOffsetType combOffset;
    int totalVolume; // 总量
    double price;

    OrderTradeStatus tradeStatus; // 交易状态
    int tradedVolume; // 已完成数量
};

// 撤单
struct OrderAction {
    OrderRefType ref;
};
