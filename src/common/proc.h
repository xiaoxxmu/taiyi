/*
 * proc.h
 *
 * Created on: 20220608
 *      Author fupeng
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
    uint32_t mdContainerId;
    uint32_t mdModuleId;
    uint32_t tradeContainerId;
    uint32_t tradeModuleId;
};

class Proc {
  public:
    static Proc *GetProcInstance() { return &_procInstance; }

  public:
    Status RegisterContainer(Container* container);
    Container* GetContainer(uint32_t containerId);

    Status RegisterInstrumentLocation(InstrumentLocation& location);
    InstrumentLocation* GetInstrumentLocation(const std::string& instrumentId);

  private:
    Proc() {}
    virtual ~Proc() {} // TODO free

  private:
    std::map<uint32_t, Container*> _containers;
    std::map<std::string, InstrumentLocation*> _locations;

  private:
    static Proc _procInstance;
};