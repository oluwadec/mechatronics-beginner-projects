# Project 01 — Closed-Loop DC Motor Position Control with Encoder + PID

**Goal:** Drive a DC motor to a commanded angular position using encoder feedback and a PID controller. Quantify performance: rise time, overshoot, steady-state error.

This is the cleanest possible introduction to real closed-loop control.

## Learning Outcomes
- Encoder quadrature decoding
- PID algorithm implementation and tuning (Kp, Ki, Kd)
- Understanding of rise time, overshoot, settling time, steady-state error
- Basic motor driver usage (H-bridge)
- Data logging via Serial for later analysis

## Hardware Required
See `BOM.md`.

Minimum viable setup:
- 1× geared DC motor with quadrature encoder (e.g. 12V 100–300 RPM with 11–20 PPR encoder)
- 1× L298N or TB6612FNG motor driver
- Arduino Uno / Nano or ESP32
- Power supply (battery or bench supply matched to motor voltage)
- Optional: potentiometer for live setpoint adjustment

## Wiring (Arduino Uno example)

```
Motor Driver (L298N)
  ENA  → Arduino D5  (PWM)
  IN1  → Arduino D6
  IN2  → Arduino D7
  OUT1/OUT2 → Motor power leads
  5V   → Arduino 5V (logic)
  GND  → common ground
  12V  → external motor supply

Encoder
  VCC  → 5V
  GND  → GND
  A    → Arduino D2  (interrupt)
  B    → Arduino D3  (interrupt)
```

**ESP32 note:** Use any two pins that support interrupts and PWM. Change pin numbers in the code.

## Mechanical Notes / CAD
- Mount the motor firmly so the shaft is free to rotate.
- Attach a simple pointer or disc to the shaft so you can visually verify position.
- Optional CAD: design a small base plate and encoder mounting bracket (see `cad/` folder).

## Software
Upload `motor_pid_position.ino`.

Key features:
- Interrupt-based encoder counting (quadrature)
- Configurable counts-per-revolution
- PID with anti-windup
- Serial commands to set target position (degrees)
- Continuous logging of time, setpoint, measured position, error, PWM output

### Serial Commands
```
T150     → set target to 150 degrees
T-90     → set target to -90 degrees
P2.5     → set Kp = 2.5
I0.1     → set Ki = 0.1
D0.05    → set Kd = 0.05
S        → print current gains and status
```

## Tuning Procedure (Ziegler-Nichols style simplified)
1. Set Ki = 0, Kd = 0. Increase Kp until the motor oscillates continuously around the setpoint. Note Ku (ultimate gain) and Pu (oscillation period).
2. Set Kp = 0.6·Ku, Ki = 1.2·Ku/Pu, Kd = 0.075·Ku·Pu (classic Ziegler-Nichols).
3. Fine-tune:
   - Too much overshoot → reduce Kp or increase Kd
   - Slow response → increase Kp
   - Steady-state error remains → increase Ki carefully
   - Oscillation → reduce Ki or increase Kd

## Performance Metrics to Measure
Run a step response from 0° to 180° (or any convenient angle).

From the logged data calculate:
- **Rise time (tr)**: time from 10% to 90% of final value
- **Overshoot (%)**: (peak – final) / final × 100
- **Settling time (ts)**: time to stay within ±2% or ±5% of final value
- **Steady-state error**: average error after settling

Example target performance for a well-tuned hobby system:
- Rise time < 0.8 s
- Overshoot < 15%
- Steady-state error < 2°

## Data Logging & Analysis
1. Open Serial Monitor / Plotter at 115200 baud.
2. Issue a step command (`T180`).
3. Copy the CSV-style output into a file.
4. Use the Python script in `shared/analysis/plot_step_response.py` to generate plots and automatic metrics.

## Common Problems & Fixes
| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Motor runs only one direction | Wiring of IN1/IN2 or encoder direction | Swap IN1/IN2 or invert encoder count |
| Encoder counts jump randomly | Missing pull-ups or noise | Enable INPUT_PULLUP, shorten wires, add 100 nF caps |
| PID output saturates | Gains too high or integral windup | Lower gains, ensure anti-windup is active |
| Steady-state error never zero | Friction / dead-zone | Increase Ki slightly or add small dither |

## Next Steps After Success
- Add velocity control inner loop (cascade PID)
- Implement trajectory following (trapezoidal profile)
- Port the same logic to ESP32 and control via Wi-Fi/Bluetooth

---
**Files in this folder**
- `motor_pid_position.ino` – main Arduino/ESP32 sketch
- `BOM.md` – parts list
- `cad/README.md` – optional mechanical design notes
