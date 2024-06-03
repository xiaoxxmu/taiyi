#include "TaiyiModule.h"

TaiyiModule *TaiyiModule::CreateTaiyiModule(TaiyiConfig *cfg) {
    if (g_pTaiyiModuleInstance == nullptr) {
        g_pTaiyiModuleInstance = TaiyiModule(cfg);
    }
    return g_pTaiyiModuleInstance;
}

TaiyiModule *TaiyiModule::GetInstance() {
    return g_pTaiyiModuleInstance;
}

TaiyiModule::TaiyiModule(const TaiyiConfig *cfg) {
    // TODO
    // _cfg = cfg; // TODO 赋值各配置项

    _taiyiMdApi = CThostFtdcMdApi::CThostFtdcMdApi();
    _taiyiTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

    for (int i = 0; i < _instrumentNum; i++) {
        string instrumentID = _pInstrumentID[i];
        TaiyiInstrumentModule *tyim = new TaiyiInstrumentModule(instrumentID);
        DBG_ASSERT(tyim);
        _instrumentModules.insert(pair<string, TaiyiInstrumentModule*>(instrumentID, tyim));
    }
}

int TaiyiModule::Start() {
    CThostFtdcMdSpi *taiyiMdSpi = new TaiyiMdSpi;
    _taiyiMdApi->RegisterSpi(taiyiMdSpi);
    _taiyiMdApi->RegisterFront(_mdFrontAddr);
    _taiyiMdApi->Init();

    // CThostFtdcTraderSpi *taiyiTraderSpi = new TaiyiTraderSpi;
    // _taiyiTraderApi->RegisterSpi(taiyiTraderSpi);
    // TODO
    // _taiyiTraderApi->Init(); 
}
