/*
 * config.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <vector>
#include <string>

struct InstrumentConfig {
    std::string instrumentId;
};

struct Config {
    uint32_t mdContainerNum;
    uint32_t tradeContainerNum;

    std::string mdFrontAddr;

    std::vector<InstrumentConfig*> instruments;

    std::string brokerId;
    std::string investerId;
    std::string investerPwd;
};
