#pragma once

#include <stdint.h>

class Time;

class Sched {
public:
  static constexpr uint8_t c_maxTasks = 8;
  typedef void (*task_cb_t)(void *data);
  struct Task {
    task_cb_t cb;
    void *data;
    uint32_t delayMs;
  };

  explicit Sched(Time &time);
  bool addTask(task_cb_t cb, void *data, uint32_t delayMs);
  void process();

private:
  Sched(const Sched &) = delete;
  void operator=(const Sched &) = delete;
  Time &time;
  uint32_t m_lastRun;
  uint8_t m_tasksTail = 0;
  Task m_tasks[c_maxTasks];
};
