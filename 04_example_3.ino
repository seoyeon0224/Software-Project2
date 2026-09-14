#define PIN_LED 13
unsigned int count;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200); // Initialize serial port
  while (!Serial) {
    ; //wait for serial port to connect.
  }
}

void loop() {
    Serial.println(++count);
    int toggle = toggle_state(count); // turn off LED.
    digitalWrite(PIN_LED, toggle) ; // Update LED status.
    delay(1000); // wait for 1,000 milliseconds
}
int toggle_state(int toggle) {
    return toggle%2;
}
