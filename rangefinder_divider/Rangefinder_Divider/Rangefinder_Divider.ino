byte com[] = {2,4,6,8,10,12,A0,A2};//A0-2
byte pins[] = {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};

void setup() {
  for (byte i = 0; i < 8; i++) {
    pinMode(com[i], OUTPUT);
    pinMode(com[i]+1, OUTPUT);
    digitalWrite(com[i], HIGH);
    digitalWrite(com[i]+1, LOW);
  }
  for (byte i = 0; i < 16; i++) {
    pinMode(pins[i], OUTPUT);
    pinMode(pins[i]+1, OUTPUT);
    digitalWrite(pins[i], HIGH);
    digitalWrite(pins[i]+1, LOW);
  }
}

void loop() {
//  for (byte i = 0; i < 8; i++) {
//    //digitalWrite(com[i], HIGH);//com[i] is already high
//    digitalWrite(com[i]+1, HIGH);
//    for (byte r = 0; r < i; r++) {
//      digitalWrite(com)
//    }
//    if (i >= 1) {
//      digitalWrite(pins[1], LOW);
//      //digitalWrite(pins[1]+1, LOW);//pins[i]+1 is already low
//    }
//    delayMicroseconds(100);
//    
//    digitalWrite(com[i], LOW);
//    digitalWrite(com[i]+1, LOW);
//    if (i >= 1) {
//      digitalWrite(pins[1], HIGH);
//      digitalWrite(pins[1]+1, HIGH);
//    }
//
//    delayMicroseconds(100);
//
//    digitalWrite(com[i], HIGH);
//    //digitalWrite(com[i]+1, LOW);//com[i]+1 is already low
//    if (i >= 7) {
//      //digitalWrite(pins[1], HIGH);//pins[i] is already high
//      digitalWrite(pins[1]+1, LOW);
//    }
//    delayMicroseconds(500);
//  }

  
  for (byte i = 0; i < 8; i++) {
    digitalWrite(com[i], HIGH);//com[i] is already high
    digitalWrite(com[i]+1, HIGH);
    if (i >= 2) {
      pinMode(pins[1], OUTPUT);
      pinMode(pins[1]+1, OUTPUT);
      digitalWrite(pins[1], LOW);
      //digitalWrite(pins[1]+1, LOW);//pins[i]+1 is already low
    }
    delayMicroseconds(600);
    
    digitalWrite(com[i], LOW);
    digitalWrite(com[i]+1, LOW);
    if (i >= 2) {
      digitalWrite(pins[1], HIGH);
      digitalWrite(pins[1]+1, HIGH);
    }

    delayMicroseconds(600);
    digitalWrite(com[i], HIGH);
    //digitalWrite(com[i]+1, LOW);//com[i]+1 is already low
    if (i >= 2) {
      //digitalWrite(pins[1], HIGH);//pins[i] is already high
      digitalWrite(pins[1]+1, LOW);
      pinMode(pins[1], INPUT);
      pinMode(pins[1]+1, INPUT);
    }
    delayMicroseconds(3000);
  }

//  digitalWrite(24, LOW);
//  digitalWrite(25, LOW);
//  delayMicroseconds(1000);
//  digitalWrite(24, HIGH);
//  digitalWrite(25, HIGH);
//  delayMicroseconds(1000);
}
