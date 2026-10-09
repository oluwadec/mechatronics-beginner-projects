# Shared CAD Templates & Guidelines

## Recommended Tools
- **Fusion 360** (free for students / hobbyists) — best overall
- **Onshape** — browser-based, good collaboration
- **FreeCAD** or **OpenSCAD** — fully open-source

## General Design Rules for These Projects
1. Design for 3D printing first (FDM): 0.2 mm layer height, 20–30 % infill, avoid large overhangs > 45°.
2. Add 0.2–0.3 mm clearance for press-fit parts (motor shafts, bearings).
3. Include mounting holes sized for M3 / M4 screws with captive nuts where possible.
4. Keep centre of mass considerations explicit for mobile and balancing robots.
5. Export both STL (printing) and STEP (sharing / future machining).

## Suggested Parametric Dimensions
- Motor mount for 25 mm / 37 mm diameter bodies
- Wheel hubs for 4–6 mm shafts
- Standard servo horn patterns (25T, 24T)

Create one master “mechatronics parts library” in your CAD package and reuse it across all four projects.
