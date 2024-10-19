void setup() {
  Serial.begin(250000);
}

void loop() {
  Serial.print(6);
  Serial.print(",");
  Serial.print(analogRead(A0));
  Serial.print(",");
  Serial.print(analogRead(A1)+2);
  Serial.print(",");
  Serial.println(analogRead(A2)+4);
}
