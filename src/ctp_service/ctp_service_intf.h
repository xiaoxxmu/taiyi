/*
 * ctp_service_intf.h
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#pragma once

#include "common.h"
#include "config.h"

class ICtpService {
  public:
    static ICtpService* CreateCtpService(Config* cfg);

  public:
    virtual void Start() = 0;

    // TODO
};
