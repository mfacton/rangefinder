struct numPins {
  byte segment[7];
};

struct numStates {
  bool segment[7];
};

struct character {
  bool segment[7];
};

bool alternate = false;

numStates states[3];
numPins num[3];
character characters[15];

void setChar (byte num, bool a, bool b, bool c, bool d, bool e, bool f, bool g) {
  characters[num].segment[0] = a;
  characters[num].segment[1] = b;
  characters[num].segment[2] = c;
  characters[num].segment[3] = d;
  characters[num].segment[4] = e;
  characters[num].segment[5] = g;
  characters[num].segment[6] = f;
}

void setDigit(byte digit, byte character) {
  for (byte s = 0; s < 7; s++) {
    states[digit].segment[s] = characters[character].segment[s];
  }
}

void setup() {
  setChar(0, 1, 1, 1, 1, 1, 1, 0);
  setChar(1, 0, 1, 1, 0, 0, 0, 0);
  setChar(2, 1, 1, 0, 1, 1, 0, 1);
  setChar(3, 1, 1, 1, 1, 0, 0, 1);
  setChar(4, 0, 1, 1, 0, 0, 1, 1);
  setChar(5, 1, 0, 1, 1, 0, 1, 1);
  setChar(6, 1, 0, 1, 1, 1, 1, 1);
  setChar(7, 1, 1, 1, 0, 0, 0, 0);
  setChar(8, 1, 1, 1, 1, 1, 1, 1);
  setChar(9, 1, 1, 1, 0, 0, 1, 1);
  setChar(10, 0, 0, 0, 0, 0, 0, 1); //aka -
  setChar(11, 1, 1, 1, 0, 1, 1, 1); //aka A
  setChar(12, 0, 0, 1, 0, 1, 0, 1); //aka n
  setChar(13, 0, 1, 1, 1, 1, 0, 1); //aka d
  setChar(14, 0, 0, 0, 0, 0, 0, 0); //aka nothing

  /*
    num[0].segment[0] = 35;
    num[0].segment[1] = 36;
    num[0].segment[2] = 30;
    num[0].segment[3] = 31;
    num[0].segment[4] = 32;
    num[0].segment[6] = 34;
    num[0].segment[5] = 33;

    num[1].segment[0] = 41;
    num[1].segment[1] = 38;
    num[1].segment[2] = 39;
    num[1].segment[3] = 14;
    num[1].segment[4] = 15;
    num[1].segment[6] = 40;
    num[1].segment[5] = 37;

    num[2].segment[0] = 42;
    num[2].segment[1] = 19;
    num[2].segment[2] = 22;
    num[2].segment[3] = 23;
    num[2].segment[4] = 24;
    num[2].segment[6] = 43;
    num[2].segment[5] = 18;
  */
  num[0].segment[0] = 42;
  num[0].segment[1] = 41;
  num[0].segment[2] = 47;
  num[0].segment[3] = 46;
  num[0].segment[4] = 45;
  num[0].segment[6] = 43;
  num[0].segment[5] = 44;

  num[1].segment[0] = 38;
  num[1].segment[1] = 37;
  num[1].segment[2] = 50;
  num[1].segment[3] = 49;
  num[1].segment[4] = 48;
  num[1].segment[6] = 39;
  num[1].segment[5] = 40;

  num[2].segment[0] = 34;
  num[2].segment[1] = 33;
  num[2].segment[2] = 53;
  num[2].segment[3] = 52;
  num[2].segment[4] = 51;
  num[2].segment[6] = 35;
  num[2].segment[5] = 36;

  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 7; s++) {
      states[n].segment[s] = false;
      pinMode(num[n].segment[s], OUTPUT);
      digitalWrite(num[n].segment[s], LOW);
    }
  }
}

void loop() {
  if (alternate) {
    for (byte n = 0; n < 3; n++) {
      for (byte s = 0; s < 7; s++) {
        if (states[n].segment[s]) {
          digitalWrite(num[n].segment[s], HIGH);
        } else {
          digitalWrite(num[n].segment[s], LOW);
        }
      }
    }
    delay(3);
  } else {
    for (byte n = 0; n < 3; n++) {
      for (byte s = 0; s < 7; s++) {
        digitalWrite(num[n].segment[s], LOW);
      }
    }
    delay(12);
  }
  alternate = !alternate;
}
