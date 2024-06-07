/*
 * store.h
 *
 * Created on: 20220608
 *      Author fupeng
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <string>

#include "module.h"

class StoreModule : public Module {
  public:
    StoreModule(uint32_t moduleId, Container* pContainer) : Module(moduleId, pContainer) {} // TODO
    ~StoreModule() {} // TODO

  public:
    Status Dispatch(Message* msg) { return StatusError; } // TODO

  private:
    std::string _filePath; // TODO
};
