# Bill of Materials — Project 01: DC Motor Position PID

| Item | Qty | Example Part | Approx. Price (USD) | Notes / Alternatives |
|------|-----|--------------|---------------------|----------------------|
| Geared DC motor + quadrature encoder | 1 | JGA25-370 or similar 12V with Hall encoder | 12–18 | 100–300 RPM ideal. Avoid very high gear ratios for first tests. |
| Motor driver | 1 | L298N module or TB6612FNG | 3–8 | TB6612 is more efficient and cooler. |
| Microcontroller | 1 | Arduino Uno / Nano or ESP32 DevKit | 5–12 | ESP32 preferred if you later want wireless. |
| Power supply | 1 | 12V 2A wall adapter or 3S LiPo | 8–15 | Must match motor voltage. Never power motor from Arduino 5V. |
| Jumper wires / breadboard | 1 set | Standard | 3–5 | |
| Optional: potentiometer 10k | 1 | Linear | 1 | Live setpoint knob. |
| Optional: 100 nF ceramic capacitors | 2–4 | | <1 | Noise suppression on encoder lines. |

**Total minimum cost:** ~$25–40

### Recommended Suppliers
- AliExpress / Amazon for motors & drivers
- Official Arduino / Espressif distributors for boards
- Local electronics shops for wire and power supplies

### Tools Needed
- Multimeter
- Soldering iron (for reliable encoder connections)
- Screwdriver / hex keys matching motor mounts
