/*
 * Project 03 — Self-Balancing Robot Starter (Complementary Filter + PID)
 * Hardware: ESP32 or Arduino + MPU-6050 + dual motors
 *
 * This is a teaching skeleton. You will need to calibrate offsets,
 * tune gains, and adjust signs for your particular wiring and chassis.
 *
 * WARNING: The robot can run away or fall. Test on soft surface with
 * power switch easily reachable.
 */

#include <Wire.h>
#include <MPU6050.h>          // install “MPU6050” by Electronic Cats or similar

MPU6050 imu;

// Motor pins (example for ESP32)
const int L_PWM = 25, L_IN1 = 26, L_IN2 = 27;
const int R_PWM = 32, R_IN1 = 33, R_IN2 = 14;

// Complementary filter
float angle = 0.0;
float alpha = 0.98;           // gyro weight
unsigned long lastTime = 0;

// PID
float Kp = 25.0, Ki = 0.0, Kd = 1.8;
float integral = 0.0;
float prevAngle = 0.0;

const float FALL_ANGLE = 35.0;  // degrees — cut motors beyond this

void setMotors(float cmd) {
  // cmd positive → drive both wheels to correct forward lean
  int pwm = constrain((int)abs(cmd), 0, 255);
  bool forward = cmd >= 0;

  digitalWrite(L_IN1, forward ? HIGH : LOW);
  digitalWrite(L_IN2, forward ? LOW  : HIGH);
  digitalWrite(R_IN1, forward ? HIGH : LOW);
  digitalWrite(R_IN2, forward ? LOW  : HIGH);

#if defined(ESP32)
  ledcWrite(0, pwm);
  ledcWrite(1, pwm);
#else
  analogWrite(L_PWM, pwm);
  analogWrite(R_PWM, pwm);
#endif
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  imu.initialize();

  if (!imu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  pinMode(L_IN1, OUTPUT); pinMode(L_IN2, OUTPUT);
  pinMode(R_IN1, OUTPUT); pinMode(R_IN2, OUTPUT);

#if defined(ESP32)
  ledcSetup(0, 20000, 8); ledcAttachPin(L_PWM, 0);
  ledcSetup(1, 20000, 8); ledcAttachPin(R_PWM, 1);
#endif

  // Simple gyro offset calibration (keep robot still)
  Serial.println("Calibrating gyro — keep robot still...");
  delay(1000);
  // (Add proper offset calculation here if desired)

  lastTime = micros();
  Serial.println("Balancer ready. Hold upright and enable.");
}

void loop() {
  // --- Read IMU ---
  int16_t ax, ay, az, gx, gy, gz;
  imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Convert to physical units (approximate)
  float accAngle = atan2(ay, az) * RAD_TO_DEG;          // adjust axes for your mounting
  float gyroRate = gx / 131.0;                          // deg/s (MPU6050 scale)

  // --- Complementary filter ---
  unsigned long now = micros();
  float dt = (now - lastTime) / 1e6;
  lastTime = now;
  if (dt <= 0 || dt > 0.05) dt = 0.01;                  // sanity

  angle = alpha * (angle + gyroRate * dt) + (1.0 - alpha) * accAngle;

  // --- Safety ---
  if (abs(angle) > FALL_ANGLE) {
    setMotors(0);
    integral = 0;
    Serial.println("FALLEN — motors cut");
    delay(100);
    return;
  }

  // --- PID ---
  float error = 0.0 - angle;                            // target = upright
  integral += error * dt;
  integral = constrain(integral, -50, 50);
  float derivative = (angle - prevAngle) / dt;          // or use gyroRate
  prevAngle = angle;

  float output = Kp * error + Ki * integral - Kd * derivative;

  setMotors(output);

  // Logging
  Serial.print(angle, 2); Serial.print(',');
  Serial.print(gyroRate, 1); Serial.print(',');
  Serial.println(output, 1);
}
