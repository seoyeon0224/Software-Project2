/*
 * Challenge 1: Parking Lot Barrier with Servo Motor and Ultrasonic Sensor
 * Software Project II - 2026
 * 
 * Features:
 * - Ultrasonic sensor detects vehicle distance
 * - Servo motor controls barrier smoothly with Ease-in-out Sine function
 * - Smooth up/down motion with trigonometric easing
 * 
 * 삼각함수 기반 가장 부드러운 제어!
 */

#include <Servo.h>
#include <math.h>

// ===== PIN Definitions =====
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// ===== Constants =====
#define DETECTION_DISTANCE 20  // 20cm - vehicle detection threshold
#define BARRIER_UP 90          // barrier up angle
#define BARRIER_DOWN 0         // barrier down angle
#define MOVING_TIME 4000       // servo movement time (4 seconds)

// ===== Global Variables =====
Servo myServo;
unsigned long moveStartTime;
int targetAngle = BARRIER_DOWN;
int currentAngle = BARRIER_DOWN;
bool isVehicleDetected = false;
bool wasVehicleDetected = false;

// ===== Ultrasonic Sensor Functions =====
long measureDistance() {
  // Send trigger pulse
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // Read echo pulse width
  long duration = pulseIn(PIN_ECHO, HIGH);
  
  // Calculate distance (duration / 2 / 29.1)
  long distance = duration / 2 / 29.1;
  
  return distance;
}

// ===== Ease-in-out Sine Function (NEW!) =====
// 삼각함수 기반의 매우 부드러운 곡선
// 수식: y = -(cos(π × t) - 1) / 2
// 특징: 가장 자연스러운 가속/감속
float easeInOutSine(float t) {
  // t: normalized value from 0 to 1
  // PI = 3.14159...
  // 결과: 0 → 1로 부드럽게 변환
  
  if (t <= 0.0) return 0.0;
  if (t >= 1.0) return 1.0;
  
  // -(cos(π*t) - 1) / 2
  return -(cos(PI * t) - 1.0) / 2.0;
}

// ===== Sigmoid Function (for comparison) =====
float sigmoid(float x, float k) {
  return 1.0 / (1.0 + exp(-k * (x - 0.5)));
}

// ===== Smoothstep Function (for comparison) =====
float smoothstep(float t) {
  return t * t * (3.0 - 2.0 * t);
}

// ===== Servo Control with Smooth Transition =====
int getSmoothAngle(unsigned long elapsed, int fromAngle, int toAngle, int functionType) {
  if (elapsed >= MOVING_TIME) {
    return toAngle;  // Movement finished
  }
  
  float progress = (float)elapsed / MOVING_TIME;  // 0.0 to 1.0
  
  float smoothProgress;
  
  switch(functionType) {
    case 1:
      // Ease-in-out Sine (NEW & BEST!)
      smoothProgress = easeInOutSine(progress);
      break;
    case 2:
      // Sigmoid
      smoothProgress = sigmoid(progress, 3.0);
      break;
    case 3:
      // Smoothstep
      smoothProgress = smoothstep(progress);
      break;
    default:
      smoothProgress = progress;  // Linear (fallback)
  }
  
  int newAngle = fromAngle + (toAngle - fromAngle) * smoothProgress;
  return newAngle;
}

// ===== Setup Function =====
void setup() {
  Serial.begin(9600);
  
  // Initialize servo
  myServo.attach(PIN_SERVO);
  myServo.write(BARRIER_DOWN);
  delay(500);
  
  // Initialize ultrasonic sensor
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  // Start movement timer
  moveStartTime = millis();
  
  Serial.println("=====================================");
  Serial.println("Parking Barrier System Started");
  Serial.println("Using: Ease-in-out Sine Function");
  Serial.println("(가장 부드러운 제어!)");
  Serial.println("=====================================");
}

// ===== Main Loop =====
void loop() {
  // Measure distance
  long distance = measureDistance();
  
  // Detect vehicle
  isVehicleDetected = (distance < DETECTION_DISTANCE && distance > 0);
  
  // Update target angle based on detection
  if (isVehicleDetected && !wasVehicleDetected) {
    // Vehicle approaching -> raise barrier
    targetAngle = BARRIER_UP;
    moveStartTime = millis();
    Serial.print("🚗 Vehicle detected! Distance: ");
    Serial.print(distance);
    Serial.println(" cm - RAISING barrier");
  } 
  else if (!isVehicleDetected && wasVehicleDetected) {
    // Vehicle passed -> lower barrier
    targetAngle = BARRIER_DOWN;
    moveStartTime = millis();
    Serial.println("✓ Vehicle passed - LOWERING barrier");
  }
  
  wasVehicleDetected = isVehicleDetected;
  
  // Calculate smooth servo position using Ease-in-out Sine
  unsigned long elapsed = millis() - moveStartTime;
  currentAngle = getSmoothAngle(elapsed, currentAngle, targetAngle, 1);  // 1 = Ease-in-out Sine
  
  // Apply servo position
  myServo.write(currentAngle);
  
  // Debug output
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Barrier Angle: ");
  Serial.println(currentAngle);
  
  delay(100);  // 100ms interval for smooth control
}
