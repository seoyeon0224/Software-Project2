// 08P07: 거리에 따른 LED 밝기제어
// 초음파 센서로 측정한 거리에 따라 LED 밝기를 삼각형 함수처럼 제어

// 핀 정의
const int PIN_TRIG = 12;
const int PIN_ECHO = 13;
const int PIN_LED = 9;

// 상수 정의
const int _DIST_MIN = 100;
const int _DIST_MAX = 300;
const int _DIST_CENTER = 200;
const int _INTERVAL = 25;

unsigned long last_sampling_time;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_LED, OUTPUT);
  
  last_sampling_time = 0;
}

void loop() {
  float distance;
  int brightness;
  
  // 샘플링 주기 체크
  if (millis() < (last_sampling_time + _INTERVAL))
    return;
  
  // 거리 측정
  distance = USS_measure(PIN_TRIG, PIN_ECHO);
  
  // 거리 값 검증 및 보정
  if ((distance == 0.0) || (distance > _DIST_MAX)) {
    distance = _DIST_MAX + 10.0;
    digitalWrite(PIN_LED, 1);
  } else if (distance < _DIST_MIN) {
    distance = _DIST_MIN - 10.0;
    digitalWrite(PIN_LED, 1);
  } else {
    // 삼각형 함수로 밝기 계산
    if (distance <= _DIST_CENTER) {
      // 100mm ~ 200mm
      brightness = (int)(255.0 * (distance - _DIST_MIN) / (_DIST_CENTER - _DIST_MIN));
    } else {
      // 200mm ~ 300mm
      brightness = (int)(255.0 * (distance - _DIST_CENTER) / (_DIST_MAX - _DIST_CENTER));
    }
    
    // 범위 제한
    if (brightness < 0) brightness = 0;
    if (brightness > 255) brightness = 255;
    
    // LED 밝기 제어
    brightness = 255 - brightness;
    analogWrite(PIN_LED, brightness);
  }
  
  // 시리얼 출력
  Serial.print("Distance:");
  Serial.print(distance, 2);
  Serial.print("mm, Brightness:");
  Serial.println(brightness);
  
  // 샘플링 시간 업데이트
  last_sampling_time += _INTERVAL;
}

// 초음파 센서 거리 측정 함수
float USS_measure(int trig_pin, int echo_pin) {
  float duration, distance;
  
  // 트리거 신호
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  
  // ECHO 핀 측정
  duration = pulseIn(echo_pin, HIGH, 30000);
  
  // 거리 계산
  distance = duration * 17.0 / 1000.0;
  distance = distance * 10.0;
  
  if (distance == 0.0) {
    return 0.0;
  }
  
  return distance;
}
