
#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <vector>
#include <atomic>

#define MAX_WAITING_MESSAGE_NUM (10000) // 队列中消息数量，如果消息入队失败，说明dst处理比较慢，src需要重试

enum LFQueueEventType {
    LFQ_EVENT_TRADE_SIGNAL = 0,
};

struct LFQueueEvent {
    uint32_t instrument_index;
    uint32_t event_type;
    uint32_t event_index;
};

class LockFreeQueue {
  public:
    LockFreeQueue(uint32_t core_id) : _lfq(MAX_WAITING_MESSAGE_NUM), _read_index(0), _write_index(0), _core_id(core_id) {}
    virtual ~LockFreeQueue() {}

    bool Push(const LFQueueEvent& item) {
        size_t cur_write_index = _write_index.load(std::memory_order_relaxed);
        size_t next_write_index = (cur_write_index + 1) % _lfq.size();
        if (next_write_index == _read_index.load(std::memory_order_acquire)) {
            return false; // 队列已满
        }
        _lfq[cur_write_index].instrument_index = item.instrument_index;
        _lfq[cur_write_index].event_type = item.event_type;
        _lfq[cur_write_index].event_index = item.event_index;
        _write_index.store(next_write_index, std::memory_order_release);
        return true;
    }

    bool Pop(LFQueueEvent& item) {
        size_t cur_read_index = _read_index.load(std::memory_order_relaxed);
        if (cur_read_index == _write_index.load(std::memory_order_acquire)) {
            return false; // 队列为空
        }
        item.instrument_index = _lfq[cur_read_index].instrument_index;
        item.event_type = _lfq[cur_read_index].event_type;
        item.event_index = _lfq[cur_read_index].event_index;
        _read_index.store((cur_read_index+1)%_lfq.size(), std::memory_order_release);
        return true;
    }

    uint32_t GetCoreId() { return _core_id; }

  private:
    std::vector<LFQueueEvent> _lfq;
    std::atomic<size_t> _read_index;
    std::atomic<size_t> _write_index;
    uint32_t _core_id;
};
