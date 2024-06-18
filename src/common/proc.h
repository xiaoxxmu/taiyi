/*
 * proc.h
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

#include "container.h"
#include "common.h"

#define TaiyiMain() Proc::GetProcInstance()

struct InstrumentLocation {
    std::string instrumentId;
    Container* mdContainer;
    Module* mdModule;
    Container* tradeContainer;
    Module* tradeModule;
};

class Proc {
  public:
    static Proc *GetProcInstance() { return &_procInstance; }

  public:
    Status RegisterContainer(Container* container);
    Container* GetContainer(uint32_t containerId);

    Status RegisterInstrumentLocation(InstrumentLocation& location);
    InstrumentLocation* GetInstrumentLocation(const std::string& instrumentId);

    void RegisterCtpService(void* ctpService) { _ctpService = ctpService; }
    void* GetCtpService() { return _ctpService; }

  private:
    Proc() {}
    virtual ~Proc() {} // TODO free

  private:
    std::map<uint32_t, Container*> _containers;
    std::map<std::string, InstrumentLocation*> _locations;

  private:
    void* _ctpService;

  private:
    static Proc _procInstance;
};
