# Project 04 — Simple 2–3 DOF Servo Arm + Gripper (Pick-and-Place)

**Goal:** Design, build and control a small articulated arm that can pick up a lightweight object and place it at a second location. Excellent CAD practice and first exposure to basic inverse kinematics.

## Learning Outcomes
- Multi-joint servo control
- Basic forward / inverse kinematics for planar arms
- Mechanical design of linkages and gripper
- Simple trajectory sequencing
- CAD → 3D print → assembly cycle

## Recommended Configuration
- **Base rotation** (optional 3rd DOF) — yaw
- **Shoulder** — major pitch joint
- **Elbow** — second pitch joint
- **Gripper** — simple two-finger or claw

A pure 2-DOF planar arm (shoulder + elbow) is perfectly sufficient for a first version and easier to analyse.

## Hardware
See `BOM.md`.

Typical parts:
- 3–4 metal-gear servos (MG996R or better for shoulder/elbow)
- 3D-printed or laser-cut links
- Simple gripper (servo + two fingers)
- Arduino / ESP32
- External 5–6 V supply for servos (do **not** power high-torque servos from Arduino 5 V)

## CAD Guidance
Link lengths (typical educational size):
- Upper arm (shoulder → elbow): 120–150 mm
- Forearm (elbow → wrist/gripper): 100–130 mm

Design rules:
- Keep joint axes parallel for planar motion
- Provide adjustable end-stops or software limits
- Make gripper fingers able to grasp a 20–30 mm cube or small ball
- Add cable routing channels so wires do not bind

Starter OpenSCAD dimensions and Fusion workflow notes are in `cad/`.

## Kinematics (Planar 2-DOF)
Forward kinematics (given θ1, θ2):
```
x = L1·cos(θ1) + L2·cos(θ1+θ2)
y = L1·sin(θ1) + L2·sin(θ1+θ2)
```

Inverse kinematics (given target x, y) — geometric solution:
```
cos(θ2) = (x² + y² – L1² – L2²) / (2·L1·L2)
θ2 = ±atan2(sin, cos)
θ1 = atan2(y, x) – atan2(L2·sin(θ2), L1 + L2·cos(θ2))
```

Implement both in code so you can command either joint angles or Cartesian targets.

## Software Features (`servo_arm.ino`)
- Joint-angle mode and simple Cartesian mode
- Smooth trajectory interpolation (linear in joint space)
- Gripper open/close commands
- Soft joint limits
- Serial command interface for teaching positions

## Build & Test Sequence
1. Print / cut all links and verify free movement by hand.
2. Mount servos, set mechanical mid-points, and record PWM centre values.
3. Write and test single-joint motion.
4. Implement forward kinematics and verify with a ruler.
5. Implement inverse kinematics and test reachability.
6. Program a pick-and-place sequence (home → pick → place → home).
7. Measure repeatability (place the same object 10 times and record scatter).

## Metrics Worth Recording
- Maximum payload before stall
- Repeatability at a fixed target (standard deviation of final position)
- Cycle time for a complete pick-and-place
- Workspace envelope (plot reachable points)

## Common Pitfalls
- Servo jitter → inadequate power supply or missing decoupling capacitors
- Arm collapses under gravity → need higher torque servos or counterbalance
- Singularity / unreachable targets → check workspace before commanding
- Cable snagging → proper strain relief and routing

---
This project pairs excellently with Project 01 (you already understand closed-loop thinking) and gives you tangible mechanical design evidence for your portfolio.
