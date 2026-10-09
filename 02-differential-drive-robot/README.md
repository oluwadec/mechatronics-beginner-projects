# Project 02 — Differential-Drive Robot (Line-Following / Obstacle-Avoiding)

**Goal:** Build a two-wheel differential-drive mobile robot that can follow a line using an IR sensor array **or** avoid obstacles using ultrasonic sensors, both under PID speed control of each wheel.

## Learning Outcomes
- Dual independent motor + encoder (or open-loop PWM) control
- Differential-drive kinematics basics
- Multi-sensor reading and simple sensor fusion
- PID applied to speed (or heading) control
- Chassis CAD and rapid prototyping

## Two Variants
You can implement either or both:
1. **Line-follower** — IR reflectance array (5 sensors typical)
2. **Obstacle avoider** — HC-SR04 ultrasonic (or two side sensors)

Both share the same chassis and motor control code.

## Hardware Overview
See `BOM.md` for full list.

Core components:
- 2× geared DC motors (ideally with encoders)
- Motor driver (TB6612FNG or L298N dual)
- Chassis (3D-printed, laser-cut acrylic, or off-the-shelf kit)
- Castor / ball wheel or third support
- IR array (TCRT5000 × 5) **or** HC-SR04
- Arduino / ESP32
- Battery pack (7.4 V or 11.1 V LiPo recommended)

## Chassis CAD Guidance
- Wheelbase: 120–160 mm
- Track width: 140–180 mm
- Ground clearance: ≥ 15 mm
- Mount motors low for stability
- Provide flat area on top for sensors and electronics

See `cad/` for dimensional recommendations and OpenSCAD starter.

## Wiring Summary (Arduino)
```
Left Motor  → Driver Channel A
Right Motor → Driver Channel B
Left Encoder A/B → D2 / D3 (or ESP32 free pins)
Right Encoder A/B → D18 / D19 (ESP32 example)

IR Array (5 sensors) → A0–A4
Ultrasonic Trig/Echo → D8 / D9

Battery → Driver power + common GND with Arduino
```

## Software Structure
- `diff_drive_base.ino` — dual PID speed control + basic differential drive commands
- Line-follower logic layered on top (weighted average of IR sensors → heading error → differential speed)
- Obstacle logic: simple reactive (turn away when distance < threshold)

## PID for Speed Control
Each wheel runs its own velocity PID:
- Setpoint = desired RPM or counts/second
- Feedback = encoder rate
- Output = PWM

For pure line following without encoders you can use open-loop PWM + differential bias; encoders give far better performance.

## Test & Metrics
- Line-follower: measure average speed on a 2 m straight line and number of losses of line on a standard track.
- Obstacle avoider: record minimum distance maintained and recovery time after obstacle detection.
- Log left/right wheel speeds and heading estimate.

## Tuning Tips
1. First tune each wheel’s speed PID independently on a stand.
2. Then place on ground and tune the outer heading controller (for line following).
3. Keep centre of mass low and between the drive wheels.

## Common Issues
| Problem | Solution |
|---------|----------|
| Robot curves unintentionally | Calibrate left/right motor dead-zone or add trim |
| IR sensors noisy | Add averaging, shield from ambient light, adjust height |
| Tip-over on turns | Lower centre of gravity, reduce max angular velocity |

---
**Next:** After this project you will have solid mobile-base experience for the self-balancing robot.
