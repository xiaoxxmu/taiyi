#include "common.h"
#include "logs.h"

#include "TaiyiInstrument.h"

TaiyiInstrumentModule::TaiyiInstrumentModule(char *instrumentID) {
    _instrumentID = instrumentID;
    _curMarketDataIndex = 0;
    for (int i := 0; i < MAX_MARKET_DATA_NUM; i++) {
        CThostFtdcDepthMarketDataField *pMarketData = (CThostFtdcDepthMarketDataField*)malloc(sizeof(CThostFtdcDepthMarketDataField));
        if !pMarketData {
            LogError("TaiyiInstrumentModule malloc failed");
            DBG_ASSERT(0); // TODO 异常处理
        }
        memset(pMarketData, 0, sizeof(CThostFtdcDepthMarketDataField));
        _ppMarketDatas[i] = pMarketData;
    }
}

void TaiyiInstrumentModule::SetMarketData(CThostFtdcDepthMarketDataField *pMarketData) {
    memcpy(_ppMarketDatas[_curMarketDataIndex], pMarketData, sizeof(CThostFtdcDepthMarketDataField));
    _curMarketDataIndex++;

    RunStrategy();
}

void TaiyiInstrumentModule::PersistMarketData() {
    // TODO 当前交易日结束时，持久化交易数据
}

void TaiyiInstrumentModule::RunStrategy() {
    // TODO 触发交易策略
}
