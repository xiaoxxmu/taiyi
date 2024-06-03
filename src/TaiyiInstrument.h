#ifndef __TAIYI_INSTRUMENT_H__
#define __TAIYI_INSTRUMENT_H__

#include "ThostFtdcUserApiDataType.h"

using namespace std;

#define MAX_MARKET_DATA_NUM 3600*2*3 // TODO 一个交易窗口总共的行情数

class TaiyiInstrumentModule {
    public:
        TaiyiInstrumentModule(char *instrumentID);
        ~TaiyiInstrumentModule() {};

        void SetMarketData(CThostFtdcDepthMarketDataField *pMarketData);

        void PersistMarketData();
    
    private:
        void RunStrategy();

    private:
        string _instrumentID;
        // TODO strategy config

    private:
        CThostFtdcDepthMarketDataField *_ppMarketDatas[MAX_MARKET_DATA_NUM];
        int _curMarketDataIndex; // 当前收到行情的位置
};

#endif
