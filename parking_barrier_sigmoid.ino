/*
 * Challenge 1: Parking Lot Barrier with Servo Motor and Ultrasonic Sensor
 * Software Project II - 2026
 * 
 * Features:
 * - Ultrasonic sensor detects vehicle distance
 * - Servo motor controls barrier smoothly with Sigmoid function
 * - Smooth up/down motion
 */

#include <Servo.h>

// ===== PIN Definitions =====
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// ===== Constants =====
#define DETECTION_DISTANCE 20  // 20cm - vehicle detection threshold
#define BARRIER_UP 90         // barrier up angle
#define BARRIER_DOWN 0         // barrier down angle
#define MOVING_TIME 6000       // servo movement time (2 seconds)
#define SMOOTH_FACTOR 3.0      // sigmoid smoothness parameter

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
  // 29.1: microseconds per cm at the speed of sound
  long distance = duration / 2 / 29.1;
  
  return distance;
}

// ===== Sigmoid Function (for smooth control) =====
float sigmoid(float x, float k) {
  // S-shaped curve: y = 1 / (1 + e^(-k*x))
  // x: normalized value from 0 to 1
  // k: smoothness factor (higher = steeper)
  return 1.0 / (1.0 + exp(-k * (x - 0.5)));
}

// ===== Smooth Step Function (alternative) =====
// Cubic easing: y = t^2 * (3 - 2*t)
float smoothstep(float t) {
  // t: normalized value from 0 to 1
  // Produces smooth acceleration/deceleration
  return t * t * (3.0 - 2.0 * t);
}

// ===== Servo Control with Smooth Transition =====
int getSmoothAngle(unsigned long elapsed, int fromAngle, int toAngle, bool useSigmoid) {
  if (elapsed >= MOVING_TIME) {
    return toAngle;  // Movement finished
  }
  
  float progress = (float)elapsed / MOVING_TIME;  // 0.0 to 1.0
  
  float smoothProgress;
  if (useSigmoid) {
    smoothProgress = sigmoid(progress, SMOOTH_FACTOR);
  } else {
    smoothProgress = smoothstep(progress);
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
  
  Serial.println("Parking Barrier System Started");
  Serial.println("Using: Sigmoid Function");
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
    Serial.print("Vehicle detected! Distance: ");
    Serial.print(distance);
    Serial.println(" cm - Raising barrier");
  } 
  else if (!isVehicleDetected && wasVehicleDetected) {
    // Vehicle passed -> lower barrier
    targetAngle = BARRIER_DOWN;
    moveStartTime = millis();
    Serial.println("Vehicle passed - Lowering barrier");
  }
  
  wasVehicleDetected = isVehicleDetected;
  
  // Calculate smooth servo position
  unsigned long elapsed = millis() - moveStartTime;
  currentAngle = getSmoothAngle(elapsed, currentAngle, targetAngle, true);  // true: use Sigmoid
  
  // Apply servo position
  myServo.write(currentAngle);
  
  // Debug output
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Barrier Angle: ");
  Serial.println(currentAngle);
  
  delay(100);  // 50ms interval for smooth control
}
