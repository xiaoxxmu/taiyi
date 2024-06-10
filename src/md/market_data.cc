/*
 * market_data.cc
 *
 * Created on: 20220608
 *      Author fupeng
 *
 */

#include "market_data.h"

#include "log.h"
#include "protocol.h"

Status MarketDataModule::Dispatch(Message* msg) {
    switch (msg->cmd) {
        case PushMarketDataReq:
            return HandlePushMarketDataReq(msg);
    }
    return StatusOK;
}




Status MarketDataModule::HandlePushMarketDataReq(Message* msg) {
    LOG_DEBUG("HandlePushMarketData");

    // TODO

    return StatusOK;
}
