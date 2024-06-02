#pragma once 

#include "ctp_api/ThostFtdcMdApi.h"

class TaiyiMdSpi: public CThostFtdcMdSpi {
    public: 
        void OnFrontConnected();
};