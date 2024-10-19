struct outPins {
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
outPins num[3];
character characters[15];

long nextTime = 0;

bool dec = false;
bool seg1 = false;
bool seg2 = false;

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
  setChar(11, 0, 0, 0, 0, 0, 0, 0); //aka nothing
  setChar(12, 1, 1, 1, 0, 1, 1, 1); //aka A
  setChar(13, 0, 0, 1, 0, 1, 0, 1); //aka n
  setChar(14, 0, 1, 1, 1, 1, 0, 1); //aka d

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

  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 7; s++) {
      states[n].segment[s] = false;
      pinMode(num[n].segment[s], OUTPUT);
    }
  }
  pinMode(27, OUTPUT);
}

void loop () {
  if (micros() >= nextTime) {
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
      digitalWrite(29, HIGH);
      nextTime = micros() + 3000;
    } else {
      for (byte n = 0; n < 3; n++) {
        for (byte s = 0; s < 7; s++) {
          digitalWrite(num[n].segment[s], LOW);
        }
      }
      digitalWrite(29,LOW);
      nextTime = micros() + 12000;
    }
    alternate = !alternate;
  }
}
