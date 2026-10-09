/*
 * Project 01 — Closed-Loop DC Motor Position Control with Encoder + PID
 * Compatible with Arduino Uno/Nano and ESP32 (change pins accordingly)
 *
 * Features:
 *  - Quadrature encoder via interrupts
 *  - PID position control with anti-windup
 *  - Serial commands for setpoint and gains
 *  - CSV-style logging for step-response analysis
 *
 * Wiring (Arduino Uno):
 *  Encoder A → D2, B → D3
 *  Motor driver ENA → D5 (PWM), IN1 → D6, IN2 → D7
 */

// ===================== PIN DEFINITIONS =====================
#if defined(ESP32)
  const int ENC_A = 18;
  const int ENC_B = 19;
  const int PWM_PIN = 25;
  const int DIR_PIN1 = 26;
  const int DIR_PIN2 = 27;
#else
  const int ENC_A = 2;   // interrupt 0
  const int ENC_B = 3;   // interrupt 1
  const int PWM_PIN = 5;
  const int DIR_PIN1 = 6;
  const int DIR_PIN2 = 7;
#endif

// ===================== MOTOR / ENCODER PARAMETERS =====================
const float COUNTS_PER_REV = 440.0;   // change to your encoder (PPR × gear ratio × 4 for quadrature)
const float DEG_PER_COUNT = 360.0 / COUNTS_PER_REV;

// ===================== PID GAINS (start conservative) =====================
float Kp = 1.8;
float Ki = 0.15;
float Kd = 0.08;

// ===================== GLOBAL STATE =====================
volatile long encoderCount = 0;
float setpointDeg = 0.0;
float integral = 0.0;
float prevError = 0.0;
unsigned long lastTimeMs = 0;
const float DT = 0.01;               // 10 ms control loop target
const float INTEGRAL_MAX = 200.0;    // anti-windup limit

// ===================== INTERRUPT SERVICE ROUTINES =====================
void IRAM_ATTR readEncoderA() {
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    encoderCount--;
  } else {
    encoderCount++;
  }
}

void IRAM_ATTR readEncoderB() {
  if (digitalRead(ENC_A) == digitalRead(ENC_B)) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

// ===================== MOTOR DRIVER =====================
void setMotor(float pwmCommand) {
  // pwmCommand: -255 … +255
  int pwm = constrain((int)abs(pwmCommand), 0, 255);

  if (pwmCommand >= 0) {
    digitalWrite(DIR_PIN1, HIGH);
    digitalWrite(DIR_PIN2, LOW);
  } else {
    digitalWrite(DIR_PIN1, LOW);
    digitalWrite(DIR_PIN2, HIGH);
  }

#if defined(ESP32)
  ledcWrite(0, pwm);          // channel 0
#else
  analogWrite(PWM_PIN, pwm);
#endif
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);

#if defined(ESP32)
  ledcSetup(0, 20000, 8);     // 20 kHz, 8-bit
  ledcAttachPin(PWM_PIN, 0);
#else
  pinMode(PWM_PIN, OUTPUT);
#endif

  attachInterrupt(digitalPinToInterrupt(ENC_A), readEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), readEncoderB, CHANGE);

  lastTimeMs = millis();

  Serial.println("DC Motor Position PID ready.");
  Serial.println("Commands: T<deg>  P<kp>  I<ki>  D<kd>  S");
  Serial.println("time_ms,setpoint_deg,position_deg,error,pwm");
}

// ===================== MAIN LOOP =====================
void loop() {
  // --- Serial command parsing ---
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("T")) {
      setpointDeg = cmd.substring(1).toFloat();
      integral = 0;               // reset integral on new setpoint
    } else if (cmd.startsWith("P")) {
      Kp = cmd.substring(1).toFloat();
    } else if (cmd.startsWith("I")) {
      Ki = cmd.substring(1).toFloat();
    } else if (cmd.startsWith("D")) {
      Kd = cmd.substring(1).toFloat();
    } else if (cmd == "S") {
      Serial.print("Kp="); Serial.print(Kp);
      Serial.print(" Ki="); Serial.print(Ki);
      Serial.print(" Kd="); Serial.print(Kd);
      Serial.print(" Setpoint="); Serial.println(setpointDeg);
    }
  }

  // --- Fixed-rate control loop ---
  unsigned long now = millis();
  if (now - lastTimeMs >= (unsigned long)(DT * 1000)) {
    lastTimeMs = now;

    noInterrupts();
    long count = encoderCount;
    interrupts();

    float positionDeg = count * DEG_PER_COUNT;
    float error = setpointDeg - positionDeg;

    // PID
    integral += error * DT;
    integral = constrain(integral, -INTEGRAL_MAX, INTEGRAL_MAX);

    float derivative = (error - prevError) / DT;
    float output = Kp * error + Ki * integral + Kd * derivative;
    prevError = error;

    setMotor(output);

    // CSV logging
    Serial.print(now);
    Serial.print(',');
    Serial.print(setpointDeg, 2);
    Serial.print(',');
    Serial.print(positionDeg, 2);
    Serial.print(',');
    Serial.print(error, 2);
    Serial.print(',');
    Serial.println(output, 1);
  }
}
