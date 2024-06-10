/*
 * lock_free_queue.h
 *
 * Created on: 20220608
 *      Author fupeng
 *
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <vector>
#include <atomic>

#include "message.h"

#define MAX_WAITING_MESSAGE_NUM (10000) // 队列中消息数量，如果消息入队失败，说明dst处理比较慢，src需要重试

class LockFreeMessageQueue {
  public:
    LockFreeMessageQueue() : _queue(MAX_WAITING_MESSAGE_NUM), _readIndex(0), _writeIndex(0) {}
    virtual ~LockFreeMessageQueue() {}

    bool Push(Message* pMsg) {
        size_t currentWriteIndex = _writeIndex.load(std::memory_order_relaxed);
        size_t nextWriteIndex = (currentWriteIndex + 1) % _queue.size();

        if (nextWriteIndex == _readIndex.load(std::memory_order_acquire)) {
            return false; // 队列已满
        }

        _queue[currentWriteIndex] = pMsg;
        _writeIndex.store(nextWriteIndex, std::memory_order_release);

        return true;
    }

    bool Pop(Message **ppMsg) {
        size_t currentReadIndex = _readIndex.load(std::memory_order_relaxed);

        if (currentReadIndex == _writeIndex.load(std::memory_order_acquire)) {
            return false; // 队列为空
        }

        *ppMsg = _queue[currentReadIndex];
        _readIndex.store((currentReadIndex+1)%_queue.size(), std::memory_order_release);

        return true;
    }

  private:
    std::vector<Message*> _queue;
    std::atomic<size_t> _readIndex;
    std::atomic<size_t> _writeIndex;
};
