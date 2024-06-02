#pragma once 

#include "ThostFtdcMdApi.h"

class TaiyiMdSpi: public CThostFtdcMdSpi {
    public: 
        void OnFrontConnected();
};