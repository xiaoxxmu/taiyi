#ifndef __TAIYI_MODULE_H__
#define __TAIYI_MODULE_H__

#include <map>

#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"

#include "common.h"

#include "TaiyiInstrument.h"

#define MAX_MD_FRONT_ADDR_LENGTH 256

using namespace std;

class TaiyiModule {
  public:
    static TaiyiModule *CreateTaiyiModule(const TaiyiConfig *cfg);
    static TaiyiModule *GetInstance();
    void Start();

    TThostFtdcBrokerIDType GetBrokerID() { return _brokerID; };
    TThostFtdcInvestorIDType GetInvesterID() { return _investerID; };
    TThostFtdcPasswordType GetInvesterPassword() { return _investerPassword; }
    char **GetInstrumentID() { return _pInstrumentID; }
    int GetInstrumentNum() { return _instrumentNum; }

    CThostFtdcMdApi *GetTaiyiMdApi() { return _taiyiMdApi; }
    CThostFtdcTraderApi *GetTaiyiTraderApi() { return _taiyiTraderApi; }

    TaiyiInstrumentModule *GetInstrumentModule(string instrumentID) { 
        auto it = _instrumentModules.find(instrumentID);
        if (it != _instrumentModules.end()) {
          return it->second;
        }
        return nullptr;
    }

  private:
    TaiyiModule(){};
    TaiyiModule(const TaiyiConfig *cfg);

  private:
    static TaiyiModule *g_pTaiyiModuleInstance; // 单例

  private:
    // TODO 配置项等
    // TaiyiConfig *_cfg;
    char _mdFrontAddr[MAX_MD_FRONT_ADDR_LENGTH];
    TThostFtdcBrokerIDType _brokerID;
    TThostFtdcInvestorIDType _investerID; // TODO id和pwd做个简单的加密，避免以明文形式出现
    TThostFtdcPasswordType _investerPassword; 
    char *_pInstrumentID[];
    int _instrumentNum;

  private:
    CThostFtdcMdApi *_taiyiMdApi;
    CThostFtdcTraderApi *_taiyiTraderApi;

  private:
    map<string, TaiyiInstrumentModule*> _instrumentModules;
};

#endif
