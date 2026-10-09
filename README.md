# Mechatronics Beginner Projects Pack
**Sensors · Actuators · Closed-Loop Control · CAD Basics**

Complete, ready-to-build package for four foundational mechatronics projects. Designed for students starting early (first-year / early second-year). Each project produces measurable performance data you can put on a resume and discuss in interviews.

## Projects Included

| # | Project | Core Skills | Estimated Cost | Time |
|---|---------|-------------|----------------|------|
| 01 | Closed-Loop DC Motor Position Control + PID | Encoder feedback, PID tuning, rise time / overshoot / steady-state error | $25–40 | 1–2 weeks |
| 02 | Differential-Drive Robot (Line / Obstacle) | Dual-motor PID speed control, IR/ultrasonic sensors, chassis CAD | $40–70 | 2–3 weeks |
| 03 | Self-Balancing Robot / Inverted Pendulum | IMU, unstable plant, state feedback / PID, reaction dynamics | $50–90 | 3–5 weeks |
| 04 | 2–3 DOF Servo Arm + Gripper | Kinematics basics, servo control, simple pick-and-place | $30–55 | 1–2 weeks |

## Recommended Build Order
1. **01 – DC Motor PID** (learn closed-loop fundamentals cleanly)
2. **04 – Servo Arm** (quick mechanical win + CAD practice)
3. **02 – Differential Drive** (combine sensors + dual actuators)
4. **03 – Self-Balancing** (hardest control problem — do last)

## Shared Requirements
- Arduino Uno / Nano **or** ESP32 (recommended for later projects)
- Basic hand tools, multimeter, soldering iron
- Access to a 3D printer (or laser cutter) is highly recommended but not mandatory for first versions
- Computer with Arduino IDE (or PlatformIO) + serial plotter

## How to Use This Repository
- Each project folder contains:
  - `README.md` – full build guide, wiring, tuning, test procedure
  - `BOM.md` – parts list with approximate prices & alternatives
  - Arduino / ESP32 code (well-commented)
  - CAD notes / OpenSCAD templates where useful
  - Data-logging & analysis guidance
- `shared/` contains reusable libraries and common CAD templates

## Portfolio Advice
For every project:
1. Record a short video (30–60 s) showing the system working.
2. Capture serial data of the key metrics (rise time, overshoot, etc.).
3. Plot the results and put them in the GitHub README.
4. Write 3–5 sentences on what failed, how you fixed it, and what you would improve.

This turns a hobby build into interview material.

## License
Open for educational use. Modify freely. Credit appreciated but not required.

---
**Start with Project 01.** Open `01-dc-motor-pid/README.md` now.
