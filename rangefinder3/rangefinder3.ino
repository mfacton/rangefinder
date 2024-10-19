typedef struct {
  unsigned int com : 4;
  unsigned int pin : 4;
} pair;

typedef struct {
  unsigned int A : 1;
  unsigned int B : 1;
  unsigned int C : 1;
  unsigned int D : 1;
  unsigned int E : 1;
  unsigned int F : 1;
  unsigned int G : 1;
} digit;

const byte comPins[] = {4, 5, 6, 7, 8, 9, 10, 11};
const byte regPins[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37};

const digit digits[] = {{1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 0, 0, 0, 0},
  {1, 1, 0, 1, 1, 0, 1},
  {1, 1, 1, 1, 0, 0, 1},
  {0, 1, 1, 0, 0, 1, 1},
  {1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 0, 0, 1, 1}
};

const pair digitPins[][4][7] = {
  {
    {
      {0, 3},
      {1, 1},
      {2, 1},
      {3, 1},
      {3, 3},
      {1, 3},
      {2, 3}
    },
    {
      {0, 5},
      {1, 4},
      {2, 4},
      {3, 4},
      {3, 5},
      {1, 5},
      {2, 5}
    },
    {
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0}
    },
    {
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0}
    }
  },
  {
    {
      {7, 2},
      {6, 1},
      {5, 1},
      {4, 1},
      {4, 2},
      {6, 2},
      {5, 2}
    },
    {
      {7, 4},
      {6, 3},
      {5, 3},
      {4, 3},
      {4, 4},
      {6, 4},
      {5, 4}
    },
    {
      {7, 6},
      {6, 5},
      {5, 5},
      {4, 5},
      {4, 6},
      {6, 6},
      {5, 6}
    },
    {
      {0, 7},
      {1, 14},
      {2, 14},
      {3, 14},
      {3, 7},
      {1, 7},
      {2, 7}
    }
  },
  {
    {
      {3, 9},
      {2, 8},
      {1, 8},
      {0, 8},
      {0, 9},
      {2, 9},
      {1, 9}
    },
    {
      {3, 11},
      {2, 10},
      {1, 10},
      {0, 10},
      {0, 11},
      {2, 11},
      {1, 11}
    },
    {
      {3, 13},
      {2, 12},
      {1, 12},
      {0, 12},
      {0, 13},
      {2, 13},
      {1, 13}
    },
    {
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0}
    }
  },
  {
    {
      {7, 9},
      {6, 8},
      {5, 8},
      {4, 8},
      {4, 9},
      {6, 9},
      {5, 9}
    },
    {
      {7, 11},
      {6, 10},
      {5, 10},
      {4, 10},
      {4, 11},
      {6, 11},
      {5, 11}
    },
    {
      {7, 13},
      {6, 12},
      {5, 12},
      {4, 12},
      {4, 13},
      {6, 13},
      {5, 13}
    },
    {
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0},
      {8, 0}
    }
  }
};

bool pins[8][16];

//if its the first byte the first bit will be 1
//second bit represents purpose
//    00000000 00000000
//num   xxxxxx xxxxxxxx


void setup() {
  //Serial.begin(115200);
  for (byte i = 0; i < 8; i++) {
    pinMode(comPins[i], OUTPUT);
    digitalWrite(comPins[i], HIGH);
  }
  pinMode(3, OUTPUT);
  
  //setDigit(1, 1, digits[7]);
  //setDigit(1, 0, digits[3]);
}

void loop() {
//  if (Serial.available()) {
//    short hold = (Serial.read()<<8) | Serial.read();
//    if (bitRead(hold, 12) && bitRead(hold, 13)) {
//      
//    }else{
//      byte row = hold >> 14;
//      short num = (hold << 2) >> 2;
//      setDigit(row, 0, digits[num % 10]);
//      num /= 10;
//      setDigit(row, 1, digits[num % 10]);
//      num /= 10;
//      setDigit(row, 2, digits[num % 10]);
//      num /= 10;
//      setDigit(row, 3, digits[num % 10]);
//    }
//  }
  
  for (byte com = 0; com < 8; com++) {
    digitalWrite(3, HIGH);
    setPins(com, LOW);
    digitalWrite(comPins[com], LOW);
    delayMicroseconds(3000);
    digitalWrite(3, LOW);
    setPins(com, HIGH);
    digitalWrite(comPins[com], HIGH);
    delayMicroseconds(3000);
    //delayMicroseconds(50);
    //delay(7);
  }
}

void setDigit(byte row, byte place, digit d) {
  if (digitPins[row][place][0].com != 8) {
    pins[digitPins[row][place][0].com][digitPins[row][place][0].pin] = d.A;
    pins[digitPins[row][place][1].com][digitPins[row][place][1].pin] = d.B;
    pins[digitPins[row][place][2].com][digitPins[row][place][2].pin] = d.C;
    pins[digitPins[row][place][3].com][digitPins[row][place][3].pin] = d.D;
    pins[digitPins[row][place][4].com][digitPins[row][place][4].pin] = d.E;
    pins[digitPins[row][place][5].com][digitPins[row][place][5].pin] = d.F;
    pins[digitPins[row][place][6].com][digitPins[row][place][6].pin] = d.G;
  }
}

void setAllOff() {
  for (byte i = 0; i < 16; i++) {
    pinMode(regPins[i], INPUT);
  }
}

void setPins(byte com, boolean onState) {
  for (byte i = 0; i < 16; i++) {
    if (pins[com][i]) {
      pinMode(regPins[i], OUTPUT);
      digitalWrite(regPins[i], onState);
    } else {
      digitalWrite(regPins[i], !onState);
    }
  }
}

///////////////////////////////////////////////////////////////////////////
/*
  01-06:speed
  01-08:WOA
  01-09:degree
  01-10:WNA
  01-11:H
  01-12:XLA
  01-13:YOD
  01-14:YOE
  01-15:YND
  01-16:YNE
  01-17:YMD
  01-18:YME
  01-19:scan
  01-20:WMG
  02-06:WOB
  02-08:WOF
  02-09:WNB
  02-10:WNF
  02-11:Q
  02-12:XLF
  02-13:YOC
  02-14:YOG
  02-15:YNC
  02-16:YNG
  02-17:YMC
  02-18:YMG
  02-19:XLB
  02-20:R
  03-06:WOC
  03-08:WOG
  03-09:WNC
  03-10:WNG
  03-11:K
  03-12:XLG
  03-13:YOB
  03-14:YOF
  03-15:YNB
  03-16:YNF
  03-17:YMB
  03-18:YMF
  03-19:XLC
  03-20:P
  04-06:WOD
  04-08:WOE
  04-09:WND
  04-10:WNE
  04-11:I
  04-12:XLE
  04-13:YM
  04-14:YOA
  04-15:flag
  04-16:YNA
  04-17:YLB & YLC
  04-18:YMA
  04-19:XLD
  04-20:J
  21-06:XOD
  21-07:XOE
  21-08:XND
  21-09:XNE
  21-10:XMD
  21-11:XME
  21-12:level
  21-13:ZOD
  21-14:ZOE
  21-15:ZND
  21-16:ZNE
  21-17:ZMD
  21-18:ZME
  22-05:XS
  22-06:XOC
  22-07:XOG
  22-08:XNC
  22-09:XNG
  22-10:XMC
  22-11:XMG
  22-12:high
  22-13:ZOC
  22-14:ZOG
  22-15:ZNC
  22-16:ZNG
  22-17:ZMC
  22-18:ZMG
  23-05:X/
  23-06:XOB
  23-07:XOF
  23-08:XNB
  23-09:XNF
  23-10:XMB
  23-11:XMF
  23-12:YY
  23-13:ZOB
  23-14:ZOF
  23-15:ZNB
  23-16:ZNF
  23-17:ZMB
  23-18:ZMF
  24-05:XY
  24-06:XM
  24-07:XOA
  24-08:decimal
  24-09:XNA
  24-10:XK & Xh
  24-11:XMA
  24-12:lightning
  24-13:ZY
  24-14:ZOA
  24-15:ZM
  24-16:ZNA
  24-17:ZLB & ZLC
  24-18:ZMA
  24-19:angle
  24-20:battery
*/
