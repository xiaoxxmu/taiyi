/*
 * taiyi_main.cc
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#include "common/proc.h"
#include "common/config.h"
#include "common/mem.h"

#include "store/store.h"
#include "md/instrument_md.h"
#include "trade/instrument_trade.h"
#include "ctp_service/ctp_service_intf.h"

Container* NewContainer(uint32_t containerId, std::string& containerName, bool isPolling) {
    Container* c = new Container(containerId, containerName, isPolling);
    return c;
}

void RegisterContainer(Config* cfg) {
    std::string storeContainerName = "store";
    Container* storeContainer = NewContainer(TAIYI_STORE_CONTAINER_ID, storeContainerName, true);
    Status ret = TaiyiMain()->RegisterContainer(storeContainer);
    DBG_ASSERT(ret == StatusOK);
    Module* storeModule = new StoreModule(TAIYI_STORE_MODULE_ID, storeContainer);
    ret = storeContainer->RegisterModule(storeModule);
    DBG_ASSERT(ret == StatusOK);

    std::vector<Container*> mdContainers(cfg->mdContainerNum);
    for (uint32_t i = 0; i < cfg->mdContainerNum; i++) {
        std::string name = "md_" + std::to_string(i);
        mdContainers[i] = NewContainer(TAIYI_MD_CONTAINER_ID+i, name, true);
        Status ret = TaiyiMain()->RegisterContainer(mdContainers[i]);
        DBG_ASSERT(ret == StatusOK);
    }
    std::vector<Container*> tradeContainers(cfg->tradeContainerNum);
    for (uint32_t i = 0; i < cfg->tradeContainerNum; i++) {
        std::string name = "trade_" + std::to_string(i);
        tradeContainers[i] = NewContainer(TAIYI_TRADE_CONTAINER_ID+i, name, true);
        Status ret = TaiyiMain()->RegisterContainer(tradeContainers[i]);
        DBG_ASSERT(ret == StatusOK);
    }

    for (uint32_t i = 0; i < cfg->instruments.size(); i++) {
        Container* mdContainer = mdContainers[i%mdContainers.size()];
        Container* tradeContainer = tradeContainers[i%tradeContainers.size()];

        Module* mdModule = new InstrumentMdModule(TAIYI_MD_MODULE_ID+i, mdContainer, cfg->instruments[i]);
        ret = mdContainer->RegisterModule(mdModule);
        DBG_ASSERT(ret == StatusOK);
        Module* tradeModule = new InstrumentTradeModule(TAIYI_TRADE_MODULE_ID+i, tradeContainer);
        ret = tradeContainer->RegisterModule(tradeModule);
        DBG_ASSERT(ret == StatusOK);

        InstrumentLocation location;
        location.instrumentId = cfg->instruments[i]->instrumentId;
        location.mdContainerId = mdContainer->GetContainerId();
        location.mdModuleId = mdModule->GetModuleId();
        location.tradeContainerId = tradeContainer->GetContainerId();
        location.tradeModuleId = tradeModule->GetModuleId();
        ret = TaiyiMain()->RegisterInstrumentLocation(location);
        DBG_ASSERT(ret == StatusOK);
    }
}

void StartContainer(Config* cfg) {
    Container* storeContainer = TaiyiMain()->GetContainer(TAIYI_STORE_CONTAINER_ID);
    Status ret = storeContainer->Start();
    DBG_ASSERT(ret == StatusOK);

    for (uint32_t i = 0; i < cfg->mdContainerNum; i++) {
        Container* mdContainer = TaiyiMain()->GetContainer(TAIYI_MD_CONTAINER_ID+i);
        ret = mdContainer->Start();
        DBG_ASSERT(ret == StatusOK);
    }

    for (uint32_t i = 0; i < cfg->tradeContainerNum; i++) {
        Container* tradeContainer = TaiyiMain()->GetContainer(TAIYI_TRADE_CONTAINER_ID+i);
        ret = tradeContainer->Start();
        DBG_ASSERT(ret == StatusOK);
    }
}

Status StartupContainer(Config* cfg) {
    RegisterContainer(cfg);
    StartContainer(cfg);
    return StatusOK;
}

Status StartCtpService(Config* cfg) {
    ICtpService* ctpService = ICtpService::CreateCtpService(cfg);
    TaiyiMain()->RegisterCtpService((void*)ctpService);
    ctpService->Start();
    return StatusOK;
}

int main(int argc, char *argv[]) {
    Config* cfg = (Config*)TAIYI_MALLOC(sizeof(Config));
    DBG_ASSERT(cfg);

    // TODO 读取配置文件
    cfg->mdContainerNum = 1;
    cfg->tradeContainerNum = 1;


    // 解析instrument配置
    InstrumentConfig insCfg;
    insCfg.instrumentId = "zn1705";
    cfg->instruments.push_back(&insCfg);

    Status ret = StartupContainer(cfg);
    DBG_ASSERT(ret == StatusOK);

    ret = StartCtpService(cfg);
    DBG_ASSERT(ret == StatusOK);

    // TODO 退出函数

    TAIYI_FREE(cfg);

    return 0;
}
