# User manual

## User interface

The clock comes with three buttons: `MODE`, `PLUS` and `MINUS`. Press `MODE` button to toggle between main modes. Hold `MODE` button to enter setup modes. `PLUS` and `MINUS` actions are relative to current mode.

Here is a mode list with corresponding button actions:

- Main modes:
    - "Clock mode":
        - `MODE` (press): go to "alarm mode"
        - `MODE` (hold): go to "clock setup mode"
        - `PLUS` (press): show date for a few seconds
        - `PLUS` (hold): show year for a few seconds
        - `MINUS` (press): show weekday for a few seconds
        - `MINUS` (hold): show ambient temperature for a few seconds
    - "Alarm view mode"
        - `MODE` (press): go to "stopwatch mode"
        - `MODE` (hold): go to "alarm setup mode"
        - `PLUS` (press): show next alarm
        - `PLUS` (hold): enable/disable current alarm
        - `MINUS` (press): show previous alarm
        - `MINUS` (hold): enable/disable current alarm
    - "Stopwatch mode"
        - `MODE` (press): go to "display test mode"
        - `MODE` (hold): reset stopwatch
        - `PLUS` (press): run or pause stopwatch
        - `MINUS` (press): run or pause stopwatch
    - "Display test mode"
        - `MODE` (press): go to "clock mode"
- Auxiliary modes:
    - "Clock setup mode" and "Alarm setup mode"
        - `MODE` (press): advance through numbers (or finish setup if it's the last number)
        - `MODE` (hold): finish setup early
        - `PLUS` (press): increase number by one
        - `PLUS` (hold): increase number by many
        - `MINUS` (press): decrease number by one
        - `MINUS` (hold): decrease number by many
    - "Alarm alert mode"
        - This one appears when an alarm fires up, press any button to stop and go to "clock mode"

## Clock calibration

DS3231 supports oscillator calibration through the "aging" register. The last screen of "Clock setup mode" sets the register value:

- The initial value is 0 (displayed as `C+00`), maximum is +127, minimum is -128.
- Positive values slow down the clock (seconds become longer)
- Negative values speed up the clock (seconds become shorter)
- Adjustment step is approximately 0.1ppm for DS3231SN or 0.12ppm for DS3231M.
- The value is retained even if the battery is taken out (there is a backup in EEPROM)
