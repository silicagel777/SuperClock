#include "sched/sched.h"
#include "driver/time/time.h"
#include "sched.h"

Sched::Sched(Time &time) : m_time(time) {
  m_lastRun = time.milliseconds();
}

bool Sched::addTask(task_cb_t cb, void *ctx, uint16_t delayMs, uint16_t reloadMs) {
  if (m_tasksTail >= c_maxTasks) {
    return false;
  }
  m_tasks[m_tasksTail] = {cb, ctx, delayMs, reloadMs};
  m_tasksTail++;
  return true;
}

inline bool Sched::removeTask(uint8_t index) {
  if (index >= m_tasksTail) {
    return false;
  }
  for (uint8_t i = index; i < m_tasksTail - 1; i++) {
    m_tasks[i] = m_tasks[i + 1];
  }
  m_tasksTail--;
  return true;
}

bool Sched::removeTasks(task_cb_t cb, void *ctx, bool useCb, bool useCtx) {
  bool result = false;
  for (uint8_t i = 0; i < m_tasksTail;) {
    if ((!useCb || m_tasks[i].cb == cb) && (!useCtx || m_tasks[i].ctx == ctx)) {
      result = true;
      removeTask(i);
    } else {
      i++;
    }
  }
  return result;
}

void Sched::run() {
  // TODO: optimize!

  // Decrement delays
  uint16_t now = m_time.milliseconds();
  uint16_t diff = now - m_lastRun;
  if (diff >= 1) {
    m_lastRun = now;
    for (uint8_t i = 0; i < m_tasksTail; i++) {
      if (m_tasks[i].delayMs > diff) {
        m_tasks[i].delayMs -= diff;
      } else {
        m_tasks[i].delayMs = 0;
      }
    }
  }

  // Run first ready task
  for (uint8_t i = 0; i < m_tasksTail; i++) {
    if (m_tasks[i].delayMs == 0) {
      Task task = m_tasks[i];
      removeTask(i);
      if (task.reloadMs > 0) {
        addTask(task.cb, task.ctx, task.reloadMs, task.reloadMs);
      }
      task.cb(task.ctx);
      break;
    }
  }
}
