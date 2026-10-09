# Project 03 — Self-Balancing Robot / Inverted Pendulum

**Goal:** Keep a two-wheeled robot upright using an IMU and closed-loop control. This is the classic unstable plant that forces deep understanding of feedback, filtering, and actuator dynamics.

## Why This Project Matters
A self-balancing robot is an inverted pendulum on wheels. Without continuous corrective action it falls. Successfully balancing it demonstrates:
- Sensor fusion (accelerometer + gyroscope)
- Complementary or Kalman filtering
- Control of an unstable system
- Real-time constraints and actuator limits

## Two Possible Implementations
1. **Two-wheel self-balancing robot** (most common, very portfolio-friendly)
2. **Reaction-wheel inverted pendulum** (single degree of freedom, cleaner dynamics for learning)

This package focuses on the two-wheel robot; the control principles transfer.

## Hardware
See `BOM.md`.

Critical parts:
- 2× good quality geared motors with encoders (or high-torque hobby motors)
- MPU-6050 or better IMU (MPU-9250 / BNO055)
- Rigid chassis with low centre of mass
- Battery placed low
- Fast microcontroller (ESP32 preferred for higher loop rates)

## Mechanical Design Priorities
- Centre of mass as low and as close to the wheel axis as possible
- Stiff chassis (no flex)
- Wheels with good traction
- Symmetric left/right mass distribution

CAD recommendations are in `cad/`.

## Control Architecture (Recommended)
1. **Inner loop:** complementary filter or simple Kalman on IMU → estimate tilt angle θ and rate θ̇
2. **Outer loop:** PID (or PD + optional integral) on angle error → desired wheel velocity or torque
3. Optional cascade: angle → velocity → motor PWM

Start with a single PID on angle:
```
error = 0 - θ          // desired upright = 0°
output = Kp*error + Ki*∫error + Kd*θ̇
```
Then map output to differential motor commands.

## Software Outline
- Read IMU at high rate (≥ 100 Hz)
- Filter → θ, θ̇
- PID → motor commands
- Safety: if |θ| > 30–40° cut motors (fallen over)
- Serial logging of angle, rate, control effort

A complete starter sketch is provided: `balance_pid.ino`.

## Tuning Strategy
1. Hold the robot vertical and verify angle reading is ~0° and stable.
2. With motors disabled, move the robot by hand and watch the estimated angle.
3. Enable motors with very low Kp. Increase Kp until it starts to oscillate, then back off.
4. Add Kd to damp oscillation.
5. Add small Ki only if a persistent lean remains.
6. Adjust for battery voltage sag and motor dead-zone.

## Performance Metrics
- Maximum tilt angle from which it recovers
- Time to recover from a small push
- RMS angle error while “stationary”
- Battery life while balancing

## Safety Notes
- Always test on a soft surface first.
- Implement a “fallen” cutoff.
- Keep hands ready to catch the robot during early tuning.

## Common Failure Modes
| Symptom | Cause | Fix |
|---------|-------|-----|
| Oscillates violently | Kp too high or derivative noisy | Lower Kp, filter gyro more |
| Slowly drifts and falls | Bias in angle estimate | Improve complementary filter alpha or calibrate IMU |
| Motors twitch at rest | Noise or dead-zone | Add dead-band or better filtering |
| Works only in one direction | Sign error in control | Invert motor direction or angle sign |

---
This project is significantly harder than the first two. Master Projects 01 and 02 first.
