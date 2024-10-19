void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    bool arr[16];
    byte count = 0;
    while (Serial.available()) {
      if (Serial.read()-48 == 0) {
        arr[count] = false;
      }else{
        arr[count] = true;
      }
      count++;
    }
    byte one;
    byte two;
    for (byte i = 0; i < 8;i++) {
      if (arr[i]) {
        bitSet(one, 7-i);
      }else{
        bitClear(one, 7-i);
      }
    }
    for (byte i = 8; i < 16;i++) {
      if (arr[i]) {
        bitSet(two, 15-i);
      }else{
        bitClear(two, 15-i);
      }
    }
    Serial.write(one);
    Serial.write(two);
  }
  delay(100);
}
