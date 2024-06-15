/*
 * container.cc
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#include "container.h"
#include "module.h"

#include "log.h"
#include "mem.h"

Container::Container(uint32_t containerId, const std::string& containerName, bool isPolling) {
    _containerId = containerId;
    _containerName = containerName;
    _isPolling = isPolling;
}

Status Container::RegisterModule(Module* pModule) {
    LOG_INFO("RegisterModule containerId %d containerName %s moduleId %d",
        _containerId, _containerName.c_str(), pModule->GetModuleId());

    std::pair<std::map<uint32_t, Module*>::iterator, bool> ret;
    ret = _modules.insert(std::pair<uint32_t, Module*>(pModule->GetModuleId(), pModule));
    if (!ret.second) {
        LOG_ERROR("RegisterModule failed, containerId %d containerName %s moduleId %d",
            _containerId, _containerName.c_str(), pModule->GetModuleId());
        return StatusError;
    }
    return StatusOK;
}

Module* Container::GetModule(uint32_t moduleId) {
    std::map<uint32_t, Module*>::iterator iter;
    iter = _modules.find(moduleId);
    if (iter == _modules.end()) {
        LOG_ERROR("GetModule failed, containerId %d containerName %s moduleId %d",
            _containerId, _containerName.c_str(), moduleId);
        return NULL;
    }
    return iter->second;
}

Status Container::Start() {
    LOG_INFO("StartContainer Id %d Name %s Polling %d", _containerId, _containerName.c_str(), _isPolling);
    if (_isPolling) {
        _thread = std::thread([this] { this->Polling(); }); // 单独启动轮询线程
    } else {
        // TODO 不启动轮询线程
    }
    return StatusOK;
}

void Container::Polling() {
    while (true) {
        TaiyiMessage* msg = NULL;
        if (_lfmQueue.Pop(&msg)) {
            Module* pModule = GetModule(msg->dstModuleId);
            if (!pModule) {
                LOG_ERROR("Recieve invalid msg, cmd %d, srcContainerId %d srcModuleId %d dstContainerId %d dstModuleId %d",
                    msg->cmd, msg->srcContainerId, msg->srcModuleId, msg->dstContainerId, msg->dstModuleId);
                continue;
            }
            pModule->Dispatch(msg);
        }
        std::this_thread::yield();
    }
}
