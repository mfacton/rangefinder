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

const byte comPins[8] = {1,2,3,4,5,6,7,8};
const byte regPins[16] = {9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

const digit digits[] = {
  {1, 1, 1, 1, 1, 1, 0},
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

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    byte b = Serial.read();
  }
  for (byte i = 0; i < 8; i++) {
    setComs(1);
    setPin(comPins[i], 3);
    for (int p = 0; p < 16; p++) {
      if (pins[i][p]) {
        setPin(regPins[p], 0);
      }else{
        setPin(regPins[p], 2);
      }
    }
    delayMicroseconds(2000);
    setComs(2);
    setPin(comPins[i], 0);
    for (int p = 0; p < 16; p++) {
      if (pins[i][p]) {
        setPin(regPins[p], 3);
      }else{
        setPin(regPins[p], 1);
      }
    }
    delayMicroseconds(2000);
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

void setComs(byte state) {
  for (byte i = 0; i < 8; i++) {
    setPin(comPins[i], state);
  }
}

void setPin(byte pin,byte state) {
  if (state == 0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }else if (state == 1) {
    pinMode(pin, INPUT);
  }else if (state == 2) {
    pinMode(pin, INPUT_PULLUP);
  }else{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
}
