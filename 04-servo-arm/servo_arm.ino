/*
 * Project 04 — Simple 2-DOF Planar Servo Arm + Gripper
 * Supports joint-angle and basic Cartesian (inverse kinematics) commands.
 *
 * Serial protocol (115200 baud):
 *   J <theta1> <theta2>     → move to joint angles (degrees)
 *   C <x> <y>               → move to Cartesian point (mm)
 *   G 0 / G 1               → gripper close / open
 *   H                       → home position
 *   S                       → print current joint angles
 */

#include <Servo.h>

// ===================== HARDWARE =====================
Servo shoulder;
Servo elbow;
Servo gripper;

const int PIN_SHOULDER = 9;
const int PIN_ELBOW    = 10;
const int PIN_GRIPPER  = 11;

// Link lengths (mm) — measure your real arm and update
const float L1 = 140.0;
const float L2 = 120.0;

// Soft limits (degrees)
const float SH_MIN = 0,   SH_MAX = 180;
const float EL_MIN = 0,   EL_MAX = 150;

// Current commanded angles
float th1 = 90.0;
float th2 = 90.0;

// ===================== KINEMATICS =====================
void forwardK(float t1, float t2, float &x, float &y) {
  float r1 = radians(t1);
  float r2 = radians(t1 + t2);
  x = L1 * cos(r1) + L2 * cos(r2);
  y = L1 * sin(r1) + L2 * sin(r2);
}

bool inverseK(float x, float y, float &t1, float &t2) {
  float dist2 = x*x + y*y;
  float c2 = (dist2 - L1*L1 - L2*L2) / (2.0 * L1 * L2);
  if (c2 < -1.0 || c2 > 1.0) return false;          // unreachable

  float s2 = sqrt(1.0 - c2*c2);                     // elbow-up solution
  t2 = degrees(atan2(s2, c2));

  float k1 = L1 + L2 * c2;
  float k2 = L2 * s2;
  t1 = degrees(atan2(y, x) - atan2(k2, k1));

  // clamp
  t1 = constrain(t1, SH_MIN, SH_MAX);
  t2 = constrain(t2, EL_MIN, EL_MAX);
  return true;
}

// ===================== MOTION =====================
void moveJoints(float target1, float target2, int steps = 40) {
  float start1 = th1, start2 = th2;
  for (int i = 1; i <= steps; i++) {
    float s = (float)i / steps;
    th1 = start1 + s * (target1 - start1);
    th2 = start2 + s * (target2 - start2);
    shoulder.write(th1);
    elbow.write(th2);
    delay(15);
  }
}

void setup() {
  Serial.begin(115200);
  shoulder.attach(PIN_SHOULDER);
  elbow.attach(PIN_ELBOW);
  gripper.attach(PIN_GRIPPER);

  // Move to safe home
  th1 = 90; th2 = 90;
  shoulder.write(th1);
  elbow.write(th2);
  gripper.write(90);          // mid position

  Serial.println("2-DOF Servo Arm ready");
  Serial.println("Commands: J t1 t2 | C x y | G 0/1 | H | S");
}

void loop() {
  if (!Serial.available()) return;

  String line = Serial.readStringUntil('\n');
  line.trim();
  if (line.length() == 0) return;

  char cmd = line.charAt(0);

  if (cmd == 'J' || cmd == 'j') {
    float a, b;
    if (sscanf(line.c_str() + 1, "%f %f", &a, &b) == 2) {
      a = constrain(a, SH_MIN, SH_MAX);
      b = constrain(b, EL_MIN, EL_MAX);
      moveJoints(a, b);
      Serial.println("OK joints");
    }
  }
  else if (cmd == 'C' || cmd == 'c') {
    float x, y, a, b;
    if (sscanf(line.c_str() + 1, "%f %f", &x, &y) == 2) {
      if (inverseK(x, y, a, b)) {
        moveJoints(a, b);
        Serial.print("OK cartesian → ");
        Serial.print(a); Serial.print(", "); Serial.println(b);
      } else {
        Serial.println("Unreachable");
      }
    }
  }
  else if (cmd == 'G' || cmd == 'g') {
    int g = line.substring(1).toInt();
    gripper.write(g == 0 ? 30 : 120);   // adjust open/close angles
    Serial.println(g == 0 ? "Gripper closed" : "Gripper open");
  }
  else if (cmd == 'H' || cmd == 'h') {
    moveJoints(90, 90);
    Serial.println("Home");
  }
  else if (cmd == 'S' || cmd == 's') {
    float x, y;
    forwardK(th1, th2, x, y);
    Serial.print("th1="); Serial.print(th1);
    Serial.print(" th2="); Serial.print(th2);
    Serial.print("  x="); Serial.print(x);
    Serial.print(" y="); Serial.println(y);
  }
}
