/*
 * config.h
 *
 * Created on: 20220608
 *      Author fupeng
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

    std::vector<InstrumentConfig*> instruments;

    bool mockCTPApi;
};
