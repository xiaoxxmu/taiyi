/*
 * proc.cc
 *
 * Created on: 20220608
 *      Author fupeng
 *
 */

#include "proc.h"

#include "log.h"
#include "mem.h"

Proc Proc::_procInstance;

Status Proc::RegisterContainer(Container* pContainer) {
    LOG_INFO("RegisterContainer id %d name: %s",
        pContainer->GetContainerId(), pContainer->GetContainerName().c_str());

    std::pair<std::map<uint32_t, Container*>::iterator, bool> ret;
    ret = _containers.insert(std::pair<uint32_t, Container*>(pContainer->GetContainerId(), pContainer));
    if (!ret.second) {
        LOG_ERROR("RegisterContainer failed, id %d name: %s",
            pContainer->GetContainerId(), pContainer->GetContainerName().c_str());
        return StatusError;
    }
    return StatusOK;
}

Container* Proc::GetContainer(uint32_t containerId) {
    std::map<uint32_t, Container*>::iterator iter;
    iter = _containers.find(containerId);
    if (iter == _containers.end()) {
        LOG_ERROR("GetContainer failed, containerId %d", containerId);
        return NULL;
    }
    return iter->second;
}

Status Proc::RegisterInstrumentLocation(InstrumentLocation& location) {
    LOG_INFO("RegisterInstrumentLocation InstrumentId: %s mdContainerId %d mdModuleId %d tradeContainerId %d tradeModuleId %d",
        location.instrumentId.c_str(), location.mdContainerId, location.mdModuleId, location.tradeContainerId, location.tradeModuleId);

    InstrumentLocation *iLoc = (InstrumentLocation*)TAIYI_MALLOC(sizeof(InstrumentLocation));
    if (!iLoc) {
        LOG_ERROR("Alloc InstrumentLocation Memory failed");
        return StatusNoMemory;
    }
    iLoc->instrumentId = location.instrumentId;
    iLoc->mdContainerId = location.mdContainerId;
    iLoc->mdModuleId = location.mdModuleId;
    iLoc->tradeContainerId = location.tradeContainerId;
    iLoc->tradeModuleId = location.tradeModuleId;

    std::pair<std::map<std::string, InstrumentLocation*>::iterator, bool> ret;
    ret = _locations.insert(std::pair<std::string, InstrumentLocation*>(iLoc->instrumentId, iLoc));
    if (!ret.second) {
        LOG_ERROR("Insert instrument location failed, id %s", iLoc->instrumentId.c_str());
        return StatusError;
    }
    return StatusOK;
}

InstrumentLocation* Proc::GetInstrumentLocation(const std::string& instrumentId) {
    std::map<std::string, InstrumentLocation*>::iterator iter;
    iter = _locations.find(instrumentId);
    if (iter == _locations.end()) {
        LOG_ERROR("GetInstrumentLocation NOT FOUND, instrumentId %s", instrumentId.c_str());
        return NULL;
    }
    return iter->second;
}
