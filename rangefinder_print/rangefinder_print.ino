const byte regPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};
const byte comPins[] = {4, 5, 6, 7, 8, 9, 10, 11};
const byte outPin = 2;
const byte inPin = 3;

void setup() {
  pinMode(outPin, OUTPUT);
  pinMode(inPin, OUTPUT);
  for (byte i = 0; i < 16; i++) {
    pinMode(regPins[i], OUTPUT);
    digitalWrite(regPins[i], LOW);
  }
  for (byte i = 0; i < 8; i++) {
    pinMode(comPins[i], OUTPUT);
    digitalWrite(comPins[i], LOW);
  }
  //analogWrite(4, 127);
}

void loop() {
//  for (byte com = 0; com < 8; com++) {
//    digitalWrite(comPins[com], LOW);//turn on com
//    writeAll(HIGH);
//    digitalWrite(outPin, HIGH);
//    digitalWrite(inPin, LOW);
//    if (com == 6) {
//      digitalWrite(regPins[1], LOW);
//    }
//    delayMicroseconds(1000);
//    writeAll(LOW);
//    digitalWrite(outPin, LOW);
//    digitalWrite(inPin, HIGH);
//    if (com == 6) {
//      digitalWrite(regPins[1], HIGH);
//    }
//    delayMicroseconds(1000);
//    digitalWrite(comPins[com], HIGH);//turn off com
//    delayMicroseconds(500);
//  }
}

void writeAll(bool state) {
  for (byte i = 0; i < 16; i++) {
    digitalWrite(regPins[i], state);
  }
}
