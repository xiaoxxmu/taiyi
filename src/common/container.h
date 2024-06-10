/*
 * container.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <thread>

#include "common.h"
#include "lock_free_queue.h"
#include "message.h"

class Module;

class Container {
  public:
    Container(uint32_t containerId, const std::string& containerName, bool isPolling);
    virtual ~Container() {} // TODO free

    uint32_t GetContainerId() { return _containerId; }
    std::string GetContainerName() { return _containerName; }

    Status RegisterModule(Module* pModule);
    Module* GetModule(uint32_t moduleId);

  public:
    Status Start();
    void Stop() {} // TODO
    void Polling();

    bool Push(Message *msg) { return _lfmQueue.Push(msg); }

  private:
    std::map<uint32_t, Module*> _modules;

  private:
    LockFreeMessageQueue _lfmQueue;

  private:
    uint32_t _containerId;
    std::string _containerName;

  private:
    bool _isPolling;
    std::thread _thread;
};
