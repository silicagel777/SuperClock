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
    uint16_t delayMs;
    uint16_t reloadMs; // zero is no reload
  };

  explicit Sched(Time &time);
  void run();
  bool addTask(task_cb_t cb, void *ctx, uint16_t delayMs, uint16_t reloadMs = 0);

  template <class C, void (C::*M)()>
  bool addTask(C *ctx, uint16_t delayMs, uint16_t reloadMs = 0) {
    return addTask(memberCb<C, M>, ctx, delayMs, reloadMs);
  }

  template <class C, void (C::*M)()>
  bool removeTasks(C *ctx) {
    return removeTasks(memberCb<C, M>, ctx, true, true);
  }

  inline bool removeTasks(task_cb_t cb, void *ctx) {
    return removeTasks(cb, ctx, true, true);
  }

  inline bool removeTasks(void *ctx) {
    return removeTasks(nullptr, ctx, false, true);
  }

  inline bool removeTasks(task_cb_t cb) {
    return removeTasks(cb, nullptr, true, false);
  }

private:
  Sched(const Sched &) = delete;
  void operator=(const Sched &) = delete;
  bool removeTask(uint8_t index);
  bool removeTasks(task_cb_t cb, void *ctx, bool useCb, bool useCtx);

  template <class C, void (C::*M)()>
  static void memberCb(void *ctx) {
    (static_cast<C *>(ctx)->*M)();
  }

  Time &m_time;
  uint16_t m_lastRun = 0;
  uint8_t m_tasksTail = 0;
  Task m_tasks[c_maxTasks];
};
