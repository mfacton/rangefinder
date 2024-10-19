#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

//long lastTime = 0;
//bool last = false;
//bool last2 = false;

//typedef struct {
//  unsigned int com : 4;
//  unsigned int pin : 4;
//} pair;

//typedef struct {
//  unsigned int A : 1;
//  unsigned int B : 1;
//  unsigned int C : 1;
//  unsigned int D : 1;
//  unsigned int E : 1;
//  unsigned int F : 1;
//  unsigned int G : 1;
//} digit;

const byte digitPins[3][7][2] = {
  {
    {3, 1},
    {2, 0},
    {1, 0},
    {0, 0},
    {0, 1},
    {2, 1},
    {1, 1}
  },
  {
    {3, 3},
    {2, 2},
    {1, 2},
    {0, 2},
    {0, 3},
    {2, 3},
    {1, 3}
  },
  {
    {3, 5},
    {2, 4},
    {1, 4},
    {0, 4},
    {0, 5},
    {2, 5},
    {1, 5}
  }
};

const char symbols[14] = {'0','1','2','3','4','5','6','7','8','9',' ','-','[',']'};

const bool digits[14][7] = {
  {true, true, true, true, true, true, false},
  {false, true, true, false, false, false, false},
  {true, true, false, true, true, false, true},
  {true, true, true, true, false, false, true},
  {false, true, true, false, false, true, true},
  {true, false, true, true, false, true, true},
  {true, false, true, true, true, true, true},
  {true, true, true, false, false, false, false},
  {true, true, true, true, true, true, true},
  {true, true, true, true, false, true, true},
  {false, false, false, false, false, false, false},
  {false, false, false, false, false, false, true},
  {true, false, false, true, true, true, false},
  {true, true, true, true, false, false, false}
};

//String segments[4][6] = {
//  {"XOD", "XOE", "XND", "XNE", "XMD", "XME"},
//  {"XOC", "XOG", "XNC", "XNG", "XMC", "XMG"},
//  {"XOB", "XOF", "XNB", "XNF", "XMB", "XMF"},
//  {"XM", "XOA", "decimal", "XNA", "XK/H", "XMA"}
//};

//123456           6,  7,  8,  9,  10, 11
//pins-            
byte pinNums[6] = {A7, A6, A5, A4, A3, A2};
byte com1 = A0;//pin 24
bool pinsG[4][6];
short lastDist = 0;

void setup() {
  Serial.begin(115200);
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
}

bool getSegment(byte digit, byte segment) {
  return pinsG[digitPins[digit][segment][0]][digitPins[digit][segment][1]];
}

byte getDigit(byte digit) {
  for (byte n = 0; n < 14; n++) {
    bool valid = true;
    for (byte s = 0; s < 7; s++) {
      if (digits[n][s] != getSegment(digit, s)) {
        valid = false;
        break;
      }
    }
    if (valid) {
      return n;
    }
  }
  return -1;
}

byte r (byte pin) {
  short num = analogRead(pin);
  if (num < 5) {
    return 0;
  } else if (num > 1020) {
    return 2;
  }
  return 1;
}

//void printDigit(byte num) {
//  if (num == 255) {
//    Serial.print(".");
//  }else{
//    Serial.print(symbols[num]);
//  }
//}

void loop() {
  bool data = false;
  bool pins[4][6] = {
    {false, false, false, false, false, false},
    {false, false, false, false, false, false},
    {false, false, false, false, false, false},
    {false, false, false, false, false, false}
  };
  byte val = r(com1);
  long ts = micros();
  if (val == 0) {
    for (byte i = 0; i < 4; i++) {
      while (micros()-ts < i*2000+1000);
      for (byte p = 0; p < 6; p++) {
        if (r(pinNums[p]) == 2) {
          pins[3 - i][p] = true;
          data = true;
        }
      }
      while (micros()-ts < (i+1)*2000);
    }
    //    Serial.print(pins[com][0]+10);
    //    Serial.print(",");
    //    Serial.print(pins[com][1]+8);
    //    Serial.print(",");
    //    Serial.print(pins[com][2]+6);
    //    Serial.print(",");
    //    Serial.print(pins[com][3]+4);
    //    Serial.print(",");
    //    Serial.print(pins[com][4]+2);
    //    Serial.print(",");
    //    Serial.println(pins[com][5]);
  }
  else if (val == 2) {
    for (byte i = 0; i < 4; i++) {
      while (micros()-ts < i*2000+1000);
      for (byte p = 0; p < 6; p++) {
        if (r(pinNums[p]) == 0) {
          pins[3 - i][p] = true;
          data = true;
        }
      }
      while (micros()-ts < (i+1)*2000);
    }
    //    Serial.print(-pins[com][0]+10);
    //    Serial.print(",");
    //    Serial.print(-pins[com][1]+8);
    //    Serial.print(",");
    //    Serial.print(-pins[com][2]+6);
    //    Serial.print(",");
    //    Serial.print(-pins[com][3]+4);
    //    Serial.print(",");
    //    Serial.print(-pins[com][4]+2);
    //    Serial.print(",");
    //    Serial.println(-pins[com][5]);
  }
  //Serial.println(millis() - tr);
  //  count_s++;
  //  if (count_s == 0) {
  //    count++;
  //    if (count == 0){
  //      com++;
  //      if (com == 4) {
  //        com = 0;
  //      }
  //    }
  //  }
  if (data) {
    for (byte c = 0; c < 4; c++) {
      for (byte p = 0; p < 6; p++) {
        pinsG[c][p] = pins[c][p];
      }
    }
//    Serial.println("--\\|/");
//    for (byte t = 0; t < 3; t++) {
//      for (byte rt = 0; rt < 7; rt++) {
//        Serial.print(getSegment(t, rt));
//      }
//      Serial.println();
//    }
    byte d2 = getDigit(2);
    byte d1 = getDigit(1);
    byte d0 = getDigit(0);
//    Serial.print("'");
//    printDigit(d2);
//    printDigit(d1);
//    printDigit(d0);
//    Serial.println("'");
    
    if (!(d0 > 10 || d1 > 10 || d2 > 10)) {
      lastDist = (d2%10) * 100 + (d1%10) * 10 + (d0%10);
      Serial.println(lastDist);
    }
  }
//    short base = r(com1);
//    short r0 = r(pinNums[0]);
//    short r1 = r(pinNums[1]);
//    short r2 = r(pinNums[2]);
//    short r3 = r(pinNums[3]);
//    short r4 = r(pinNums[4]);
//    short r5 = r(pinNums[5]);

//    Serial.print(base);
//    Serial.print(",");
//    Serial.print(r0+3);
//    Serial.print(",");
//    Serial.print(r1+6);
//    Serial.print(",");
//    Serial.print(r2+9);
//    Serial.print(",");
//    Serial.print(r3+12);
//    Serial.print(",");
//    Serial.print(r4+15);
//    Serial.print(",");
//    Serial.print(r5+18);
//    Serial.println();
  //
//    Serial.print(24);
//    Serial.print(",");
//    Serial.print(r0-base+20);
//    Serial.print(",");
//    Serial.print(r1-base+16);
//    Serial.print(",");
//    Serial.print(r2-base+12);
//    Serial.print(",");
//    Serial.print(r3-base+8);
//    Serial.print(",");
//    Serial.print(r4-base+4);
//    Serial.print(",");
//    Serial.print(r5-base);
//    Serial.println();

  // 1960
  //25400
  //31800
  //  Serial.print(analogRead(A0)+10);
  //  Serial.print(",");
  //  Serial.print(analogRead(A1));
  //  Serial.print(",");
  //  Serial.print(analogRead(A2));
  //  Serial.print(",");
  //  Serial.print(analogRead(A3));
  //  Serial.println();
  //  short a1 = analogRead(A0);
  //  short a2 = analogRead(A1);
  //  short a3 = analogRead(A2);
  //  short a4 = analogRead(A3);
  //  bool c = a4 == 1023;
  //  bool c2 = a1 == 1023;
  //  if (c2 && !last2) {
  //    lastTime = micros();
  //  }
  //  if (!c2 && last2) {
  //    Serial.println(micros()- lastTime);
  //  }
  //  last2 =c2;

  //  Serial.print(6144);
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[0])+5120));
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[1])+4096));
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[2])+3072));
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[3])+2048));
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[4])+1024));
  //  Serial.print(",");
  //  Serial.print((analogRead(pinNums[5])));
  //  Serial.println();

  //  Serial.print((digitalRead(pinNums[0])+10));
  //  Serial.print(",");
  //  Serial.print((digitalRead(pinNums[1])+8));
  //  Serial.print(",");
  //  Serial.print((digitalRead(pinNums[2])+6));
  //  Serial.print(",");
  //  Serial.print((digitalRead(pinNums[3])+4));
  //  Serial.print(",");
  //  Serial.print((digitalRead(pinNums[4])+2));
  //  Serial.print(",");
  //  Serial.print((digitalRead(pinNums[5])));
  //  Serial.println();

  //  short thresh = 685;
  //  Serial.print(((analogRead(pinNums[0])>thresh)+10));
  //  Serial.print(",");
  //  Serial.print(((analogRead(pinNums[1])>thresh)+8));
  //  Serial.print(",");
  //  Serial.print(((analogRead(pinNums[2])>thresh)+6));
  //  Serial.print(",");
  //  Serial.print(((analogRead(pinNums[3])>thresh)+4));
  //  Serial.print(",");
  //  Serial.print(((analogRead(pinNums[4])>thresh)+2));
  //  Serial.print(",");
  //  Serial.print(((analogRead(pinNums[5])>thresh)));
  //  Serial.println();

  //  Serial.print(analogRead(A2));
  //  Serial.print(",");
  //  Serial.print(analogRead(A3));
  //  Serial.print(",");
  //  Serial.print(analogRead(A4));
  //  Serial.print(",");
  //  Serial.print(analogRead(A5));
  //  Serial.print(",");
  //  Serial.print(analogRead(A6));
  //  Serial.print(",");
  //  Serial.println(analogRead(A7));
}

//void loop() {
//  bool pins[4][6] = {
//    {false, false, false, false, false, false},
//    {false, false, false, false, false, false},
//    {false, false, false, false, false, false},
//    {false, false, false, false, false, false}
//  };
//  for (byte c = 0; c < 4; c++) {
//    digitalWrite(comNums[c], HIGH);
//    delay(2);
//    long t = millis();
//    while (millis() - t < 35) {
//      for (byte p = 0; p < 6; p++) {
//        if (analogRead(pinNums[p])> 700) {
//          pins[c][p] = true;
//        }
//      }
//    }
//    digitalWrite(comNums[c], LOW);
//  }
//  for (byte c = 0; c < 4; c++) {
//    for (byte p = 0; p < 6; p++) {
//      pinsG[c][p] = pins[c][p];
//    }
//  }
////  for (byte c = 0; c < 4; c++) {
////    for (byte p = 0; p < 6; p++) {
////      if (pins[c][p]) {
////        Serial.println(segments[c][p]);
////      }
////    }
////  }
////  Serial.println("-");
//  byte d2 = getDigit(2);
//  byte d1 = getDigit(1);
//  byte d0 = getDigit(0);
//  if (!(d0 == 255 || d1 == 255 || d2 == 255)) {
//    lastDist = d2*100+d1*10+d0;
//    Serial.println(lastDist);
//  }
//}

//1:1-XOD
//1:2-XOE
//1:3-XND
//1:4-XNE
//1:5-XMD
//1:6-XME
//2:1-XOC
//2:2-XOG
//2:3-XNC
//2:4-XNG
//2:5-XMC
//2:6-XMG
//3:1-XOB
//3:2-XOF
//3:3-XNB
//3:4-XNF
//3:5-XMB
//3:6-XMF
//4:2-XOA
//4:4-XNA
//4:6-XMA
