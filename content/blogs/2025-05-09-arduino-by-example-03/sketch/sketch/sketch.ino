void setup() {
  // initialize digital pin LED_BUILDIN as an output
  pinMode(LED_BUILTIN, OUTPUT)
}

void loop() {
  // turn on the LED
  digitalWrite(LED_BUILTIN, HIGH)
  // wait for a second
  delay(1000)
  // turn off the LED
  digitalWrite(LED_BUILTIN, LOW)
  // wait for a second
  delay(1000)
}
