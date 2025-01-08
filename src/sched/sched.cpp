#include "sched/sched.h"
#include "driver/time/time.h"

Sched::Sched(Time &time) : time(time) {
  m_lastRun = time.milliseconds();
}

bool Sched::addTask(task_cb_t cb, void *data, uint32_t delayMs) {
  if (m_tasksTail >= c_maxTasks) {
    return false;
  }
  m_tasks[m_tasksTail] = {cb, data, delayMs};
  m_tasksTail++;
  return true;
}

void Sched::process() {
  // TODO: optimize!
  uint32_t now = time.milliseconds();
  uint32_t diff = now - m_lastRun;
  if (diff >= 1) {
    m_lastRun = now;
    for (uint8_t i = 0; i < m_tasksTail;) {
      if (m_tasks[i].delayMs > diff) {
        m_tasks[i].delayMs -= diff;
        i++;
      } else {
        m_tasks[i].cb(m_tasks[i].data);
        for (uint8_t j = i; j < m_tasksTail - 1; j++) {
          m_tasks[j] = m_tasks[j + 1];
        }
        m_tasksTail--;
      }
    }
  }
}
