/*
 * instrument_md.cc
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#include "instrument_md.h"

#include "log.h"
#include "protocol.h"
#include "mem.h"
#include "common.h"
#include "message.h"

InstrumentMdModule::InstrumentMdModule(uint32_t moduleId, Container* pContainer, InstrumentConfig* cfg) : Module(moduleId, pContainer) {
    _cfg = cfg;
    _mdNum = 0;
    _location = NULL;

    _pMsgPool = new TaiyiMemPool(MD_NUM_PER_INSTRUMENT, sizeof(TaiyiMessage));
    DBG_ASSERT(_pMsgPool);
    _pTradeReqPool = new TaiyiMemPool(MD_NUM_PER_INSTRUMENT, sizeof(InstrumentTradeSignalReq));
    DBG_ASSERT(_pTradeReqPool);
}

Status InstrumentMdModule::Dispatch(TaiyiMessage* msg) {
    switch (msg->cmd) {
        case PushMarketDataReqCmd:
            return HandlePushMarketDataReq(msg);
    }
    return StatusOK;
}

Status InstrumentMdModule::HandlePushMarketDataReq(TaiyiMessage* msg) {
    _pMds[_mdNum++] = (CThostFtdcDepthMarketDataField*)msg->data[0];
    int signal = DoStrategy();
    SendTradeSignal(signal);
    return StatusOK;
}

int InstrumentMdModule::DoStrategy() { // TODO
    return 0;
}

void InstrumentMdModule::SendTradeSignal(int signal) {
    TaiyiMessage* msg = (TaiyiMessage*)_pMsgPool->Zalloc();
    DBG_ASSERT(msg);

    InstrumentLocation* loc = GetLocation();
    msg->srcContainerId = GetContainer()->GetContainerId();
    msg->srcModuleId = GetModuleId();
    msg->dstContainerId = loc->tradeContainer->GetContainerId();
    msg->dstModuleId = loc->tradeModule->GetModuleId();

    InstrumentTradeSignalReq* req = (InstrumentTradeSignalReq*)_pTradeReqPool->Zalloc();
    DBG_ASSERT(req);
    req->signal = signal;
    req->curMdNum = GetCurMdNum();

    msg->cmd = PushTradeSignalReqCmd;
    msg->data[0] = (void*)req;

    if (StatusOK != SendMsg(msg)) {
        LOG_ERROR("SendTradeSignal failed, instrumentId %s", _cfg->instrumentId.c_str());
    }
}
