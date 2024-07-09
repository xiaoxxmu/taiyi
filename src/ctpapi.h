#pragma once

#include "common.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

void Start();
void Wait();


void SetTradeFrontId(TThostFtdcFrontIDType trade_front_id);
TThostFtdcFrontIDType GetTradeFrontId();

void SetSessionId(TThostFtdcSessionIDType session_id);
TThostFtdcSessionIDType GetSessionId();

void SetOrderRef(uint32_t order_ref);
uint32_t GetOrderRef();
