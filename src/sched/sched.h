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
  inline bool removeTasks(void *data) {
    return removeTasks(nullptr, data, false, true);
  }
  inline bool removeTasks(task_cb_t cb) {
    return removeTasks(cb, nullptr, true, false);
  }
  inline bool removeTasks(task_cb_t cb, void *data) {
    return removeTasks(cb, data, true, true);
  }
  void run();

private:
  Sched(const Sched &) = delete;
  void operator=(const Sched &) = delete;
  bool removeTask(uint8_t index);
  bool removeTasks(task_cb_t cb, void *data, bool useCb, bool useData);

  Time &m_time;
  uint32_t m_lastRun = 0;
  uint8_t m_tasksTail = 0;
  Task m_tasks[c_maxTasks];
};
