#pragma once

#include "taiyi_data.h"

/// @brief 记录所有收到的行情
class TaiyiMarketDataModule
{
public:
    TaiyiMarketDataModule() : md_cnt(0) {}
    ~TaiyiMarketDataModule() {}

    void Run() {} // TODO

private:
    taiyi_market_data_t mds[800000];
    uint32_t md_cnt;
};

// void set_ctp_market_data(void *md); // ctp
// void set_rem_market_data(void *md); // 盛立
