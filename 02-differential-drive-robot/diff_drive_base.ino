/*
 * Project 02 — Differential Drive Base with Dual PID Speed Control
 * Supports both line-following and obstacle-avoidance layers.
 *
 * This sketch provides:
 *  - Independent left/right velocity PID
 *  - Simple differential drive kinematics (v, omega → left/right speed)
 *  - Placeholder hooks for IR array or ultrasonic logic
 *
 * Wiring notes are in the project README.
 */

#include <Arduino.h>

// ===================== PIN CONFIG (Arduino Uno example) =====================
const int LEFT_PWM  = 5;
const int LEFT_IN1  = 6;
const int LEFT_IN2  = 7;
const int RIGHT_PWM = 9;
const int RIGHT_IN1 = 10;
const int RIGHT_IN2 = 11;

const int LEFT_ENC_A  = 2;
const int LEFT_ENC_B  = 3;
const int RIGHT_ENC_A = 18;   // change for Uno if needed (use software)
const int RIGHT_ENC_B = 19;

// IR array pins (A0–A4) or ultrasonic
const int IR_PINS[5] = {A0, A1, A2, A3, A4};
const int TRIG_PIN = 8;
const int ECHO_PIN = 12;

// ===================== PARAMETERS =====================
const float COUNTS_PER_REV = 440.0;
const float WHEEL_RADIUS   = 0.032;   // m
const float WHEEL_BASE     = 0.15;    // m distance between wheels

// PID gains for speed (counts per second)
float Kp_s = 0.8, Ki_s = 0.4, Kd_s = 0.02;

// ===================== STATE =====================
volatile long leftCount = 0, rightCount = 0;
float leftIntegral = 0, rightIntegral = 0;
float leftPrevErr = 0, rightPrevErr = 0;
float leftSpeedSet = 0, rightSpeedSet = 0;   // counts/s

unsigned long lastControlMs = 0;
const float DT = 0.02;

// ===================== ENCODER ISRs =====================
void leftEncISR() {
  if (digitalRead(LEFT_ENC_A) == digitalRead(LEFT_ENC_B)) leftCount--;
  else leftCount++;
}
void rightEncISR() {
  if (digitalRead(RIGHT_ENC_A) == digitalRead(RIGHT_ENC_B)) rightCount--;
  else rightCount++;
}

// ===================== MOTOR HELPERS =====================
void setLeftMotor(float cmd) {
  int pwm = constrain((int)abs(cmd), 0, 255);
  if (cmd >= 0) { digitalWrite(LEFT_IN1, HIGH); digitalWrite(LEFT_IN2, LOW); }
  else          { digitalWrite(LEFT_IN1, LOW);  digitalWrite(LEFT_IN2, HIGH); }
  analogWrite(LEFT_PWM, pwm);
}
void setRightMotor(float cmd) {
  int pwm = constrain((int)abs(cmd), 0, 255);
  if (cmd >= 0) { digitalWrite(RIGHT_IN1, HIGH); digitalWrite(RIGHT_IN2, LOW); }
  else          { digitalWrite(RIGHT_IN1, LOW);  digitalWrite(RIGHT_IN2, HIGH); }
  analogWrite(RIGHT_PWM, pwm);
}

// Convert linear velocity (m/s) and angular velocity (rad/s) to wheel speeds
void setVelocity(float v, float omega) {
  float leftV  = v - omega * WHEEL_BASE / 2.0;
  float rightV = v + omega * WHEEL_BASE / 2.0;
  // convert m/s → counts/s
  float cps_per_mps = COUNTS_PER_REV / (2.0 * PI * WHEEL_RADIUS);
  leftSpeedSet  = leftV  * cps_per_mps;
  rightSpeedSet = rightV * cps_per_mps;
}

// ===================== SIMPLE SENSOR READERS =====================
float readUltrasonic() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long us = pulseIn(ECHO_PIN, HIGH, 30000);
  return us * 0.0343 / 2.0;   // cm
}

int readIRArray(int values[5]) {
  int sum = 0, weighted = 0;
  for (int i = 0; i < 5; i++) {
    values[i] = analogRead(IR_PINS[i]);
    // assume lower reading = on line (adjust threshold for your sensors)
    if (values[i] < 500) {
      sum += 1;
      weighted += (i - 2) * 100;   // -200 … +200
    }
  }
  if (sum == 0) return 0;          // lost line
  return weighted / sum;
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);

  pinMode(LEFT_IN1, OUTPUT); pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT); pinMode(RIGHT_IN2, OUTPUT);
  pinMode(LEFT_ENC_A, INPUT_PULLUP); pinMode(LEFT_ENC_B, INPUT_PULLUP);
  pinMode(RIGHT_ENC_A, INPUT_PULLUP); pinMode(RIGHT_ENC_B, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);

  attachInterrupt(digitalPinToInterrupt(LEFT_ENC_A), leftEncISR, CHANGE);
  // For Uno you may need PinChangeInterrupt library for right encoder

  lastControlMs = millis();
  Serial.println("Differential drive base ready");
}

// ===================== MAIN LOOP =====================
void loop() {
  unsigned long now = millis();
  if (now - lastControlMs < (unsigned long)(DT * 1000)) return;
  lastControlMs = now;

  // --- Example behaviour selection ---
  // Uncomment one of the following blocks

  // 1. Open-loop straight line
  // setVelocity(0.15, 0.0);

  // 2. Simple obstacle avoidance
  float dist = readUltrasonic();
  if (dist < 25.0 && dist > 0.5) {
    setVelocity(0.0, 1.2);          // turn in place
  } else {
    setVelocity(0.18, 0.0);         // go forward
  }

  // 3. Line following (basic)
  /*
  int ir[5];
  int headingError = readIRArray(ir);
  float omega = -0.008 * headingError;   // tune this gain
  setVelocity(0.12, omega);
  */

  // --- Speed PID for each wheel ---
  static long prevLeft = 0, prevRight = 0;
  long curLeft = leftCount, curRight = rightCount;
  float leftSpeed  = (curLeft  - prevLeft)  / DT;
  float rightSpeed = (curRight - prevRight) / DT;
  prevLeft = curLeft; prevRight = curRight;

  // Left PID
  float lErr = leftSpeedSet - leftSpeed;
  leftIntegral += lErr * DT;
  leftIntegral = constrain(leftIntegral, -300, 300);
  float lDer = (lErr - leftPrevErr) / DT;
  float lOut = Kp_s * lErr + Ki_s * leftIntegral + Kd_s * lDer;
  leftPrevErr = lErr;
  setLeftMotor(lOut);

  // Right PID
  float rErr = rightSpeedSet - rightSpeed;
  rightIntegral += rErr * DT;
  rightIntegral = constrain(rightIntegral, -300, 300);
  float rDer = (rErr - rightPrevErr) / DT;
  float rOut = Kp_s * rErr + Ki_s * rightIntegral + Kd_s * rDer;
  rightPrevErr = rErr;
  setRightMotor(rOut);

  // Optional logging
  // Serial.print(leftSpeed); Serial.print(','); Serial.println(rightSpeed);
}
