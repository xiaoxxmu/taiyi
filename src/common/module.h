/*
 * module.h
 *
 * Created on: 20240608
 *      Author xiaoxxmu
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include "message.h"
#include "container.h"

class Module {
  public:
    Module(uint32_t moduleId, Container* pContainer) : _moduleId(moduleId), _container(pContainer) {}
    virtual ~Module() {} // TODO

    uint32_t GetModuleId() { return _moduleId; }
    Container* GetContainer() { return _container; }

  public:
    virtual Status Dispatch(TaiyiMessage* msg) = 0;
    Status SendMsg(TaiyiMessage* msg);

  private:
    uint32_t _moduleId;
    Container* _container;
};
