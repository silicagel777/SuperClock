#pragma once

class Tone {
public:
  explicit Tone();
  ~Tone();
  virtual void run(uint16_t freq);
  virtual void stop();

private:
  Tone(const Tone &) = delete;
  void operator=(const Tone &) = delete;
};
