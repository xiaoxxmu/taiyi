/*
 * module.cc
 *
 * Created on: 20220608
 *      Author fupeng
 *
 */

#include "module.h"
#include "log.h"
#include "mem.h"
#include "container.h"
#include "proc.h"

Status Module::SendMsg(Message* msg) {
    Container* dstContainer = TaiyiMain()->GetContainer(msg->dstContainerId);
    if (dstContainer->Push(msg)) {
        return StatusOK;
    }
    return StatusError; // TODO 队列满，异常处理
}
