void setup() {
  // begin serial communications, at 9600 bits of data per second, between the board and the computer
  Serial.begin(9600);

}

void loop() {
  // read analog value from A0 into variable `sensorValue`
  int sensorValue = analogRead(A0);

  // print this information to your serial monitor window. 
  Serial.println(sensorValue);
}
