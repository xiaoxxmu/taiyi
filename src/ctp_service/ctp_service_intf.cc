/*
 * ctp_service_intf.cc
 *
 * Created on: 20240612
 *      Author xiaoxxmu
 *
 */

#include "ctp_service_intf.h"
#include "ctp_service.h"

ICtpService* ICtpService::CreateCtpService(Config* cfg) {
    ICtpService* ctpService = new CtpService(cfg);
    return ctpService;
}
