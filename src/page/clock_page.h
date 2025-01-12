#pragma once

#include <driver/button/button.h>
#include <page/ipage.h>

class Buzzer;
class Display;
class IRtc;
class ITemp;
class Sched;

class ClockPage : public IPage {
public:
  ClockPage(Sched &sched, Display &display, Buzzer &buzzer, Button &button, IRtc &rtc, ITemp &temp);
  void show() override;
  void hide() override;

private:
  ClockPage(const ClockPage &) = delete;
  void operator=(const ClockPage &) = delete;
  static void buttonCallback(void *self, Button::Type type, Button::State state);
  void handleButton(Button::Type type, Button::State state);
  static void showTimeCallback(void *self);
  void showTime();
  void showDate();
  void showTemp();
  void showWeek();
  void showYear();
  static constexpr uint16_t c_timeRefreshDelay = 500;
  static constexpr uint16_t c_returnToTimeDelay = 2000;
  Sched &m_sched;
  Display &m_display;
  Buzzer &m_buzzer;
  Button &m_button;
  IRtc &m_rtc;
  ITemp &m_temp;
};
