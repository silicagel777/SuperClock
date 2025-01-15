#pragma once

#include <stdint.h>

class Time;

class Sched {
public:
  static constexpr uint8_t c_maxTasks = 12;
  typedef void (*task_cb_t)(void *ctx);
  struct Task {
    task_cb_t cb;
    void *ctx;
    uint32_t delayMs;
  };

  explicit Sched(Time &time);
  void run();
  bool addTask(task_cb_t cb, void *ctx, uint32_t delayMs);

  template <class C, void (C::*M)()>
  bool addTask(C *ctx, uint32_t delayMs) {
    auto cb = [](void *ctx) { (((C *)ctx)->*M)(); };
    return addTask(cb, ctx, delayMs);
  }

  inline bool removeTasks(void *ctx) {
    return removeTasks(nullptr, ctx, false, true);
  }

  inline bool removeTasks(task_cb_t cb) {
    return removeTasks(cb, nullptr, true, false);
  }

  inline bool removeTasks(task_cb_t cb, void *ctx) {
    return removeTasks(cb, ctx, true, true);
  }

private:
  Sched(const Sched &) = delete;
  void operator=(const Sched &) = delete;
  bool removeTask(uint8_t index);
  bool removeTasks(task_cb_t cb, void *ctx, bool useCb, bool useCtx);

  Time &m_time;
  uint32_t m_lastRun = 0;
  uint8_t m_tasksTail = 0;
  Task m_tasks[c_maxTasks];
};
