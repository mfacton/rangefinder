byte b1 = 5;
byte b2 = 4;
byte refPin = 44;
byte decRead = 48;
byte decWrite = 25;
byte batt1 = 27;
byte batt2 = 26;

bool lastRef = false;

bool displayDecimal = false;
bool displayBatt1 = false;
bool displayBatt2 = false;

bool battFlipTime;

long b2Start;
bool lastb2 = false;
bool b2Hold = false;
bool distToggle = false;

bool readScreen = true;

bool alternate = false;
long nextTime = 0;

double d1;
double d2;
double a1;
double a2;
double res;

byte screen[3];
double lastRead;

struct outPins {
  byte segment[7];
};

struct numStates {
  bool segment[7];
};

struct character {
  bool segment[7];
};

struct inPins {
  byte segment[6];
};

struct pinReads {
  byte segment[6];
};

numStates states[3];
outPins num[3];
character characters[15];
pinReads reads[3];
inPins inPinNum[3];

double calculateDist (double angle1, double angle2, double dist1, double dist2) {
  return sqrt(pow(dist1, 2) + pow(dist2, 2) - 2 * dist1 * dist2 * cos(M_PI * abs(angle1 - angle2) / 180));
}

bool checkChar(byte n, byte c) {
  for (byte s = 0; s < 6; s++) {
    if (reads[n].segment[s] != characters[c].segment[s]) {
      return false;
    }
  }
  return true;
}

byte checkNum(byte n) {
  for (byte c = 0; c < 12; c++) {
    if (checkChar(n, c)) {
      return c;
    }
  }
  return 11;
}

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

void validate() {
  String s;
  for (byte n = 0; n < 3; n++) {
    if (screen[n] >= 10) {
      return;
    }
    s += (String)screen[n];
  }
  if (displayDecimal) {
    lastRead = s.toDouble() / 10.0;
  } else {
    lastRead = s.toDouble();
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

  inPinNum[0].segment[0] = 6;
  inPinNum[0].segment[1] = 7;
  inPinNum[0].segment[2] = 10;
  inPinNum[0].segment[3] = 3;
  inPinNum[0].segment[4] = 17;
  inPinNum[0].segment[5] = 16;

  inPinNum[1].segment[0] = 9;
  inPinNum[1].segment[1] = 53;
  inPinNum[1].segment[2] = 13;
  inPinNum[1].segment[3] = 12;
  inPinNum[1].segment[4] = 11;
  inPinNum[1].segment[5] = 8;

  inPinNum[2].segment[0] = 51;
  inPinNum[2].segment[1] = 50;
  inPinNum[2].segment[2] = 45;
  inPinNum[2].segment[3] = 46;
  inPinNum[2].segment[4] = 47;
  inPinNum[2].segment[5] = 52;

  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(refPin, INPUT);
  pinMode(decRead, INPUT);
  pinMode(decWrite, OUTPUT);
  pinMode(batt1, OUTPUT);
  pinMode(batt2, OUTPUT);

  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 6; s++) {
      pinMode(inPinNum[n].segment[s], INPUT);
    }
  }

  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 7; s++) {
      states[n].segment[s] = false;
      pinMode(num[n].segment[s], OUTPUT);
      digitalWrite(num[n].segment[s], LOW);
    }
  }
  Serial.begin(9600);
}

String numText(int num) {
  switch (num) {
    case 10:
      return "-";
    case 11:
      return " ";
    default:
      return num + "";
  }
}

String screenText() {
  String s = "";
  for (int i = 0; i < 3; i++) {
    s += numText(screen[i]);
  }
  return s;
}

void loop () {
  if (readScreen) {
    if (digitalRead(refPin) == HIGH) {
      if (!lastRef) {
        lastRef = true;
      }
    } else {
      if (lastRef) {
        for (byte n = 0; n < 3; n++) {
          for (byte s = 0; s < 6; s++) {
            reads[n].segment[s] = digitalRead(inPinNum[n].segment[s]);
          }
        }
        displayDecimal = digitalRead(decRead);

        for (byte n = 0; n < 3; n++) {
          screen[n] = checkNum(n);
        }
        lastRef = false;
      }
    }
  }
  Serial.print("'");
  Serial.print(screen[0]);
  Serial.print(",");
  Serial.print(screen[1]);
  Serial.print(",");
  Serial.print(screen[2]);
  Serial.println("'");
  
  validate();

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
      digitalWrite(batt1, displayBatt1);
      digitalWrite(batt2, displayBatt2);
      digitalWrite(decWrite, displayDecimal);

      nextTime = micros() + 3000;
    } else {
      for (byte n = 0; n < 3; n++) {
        for (byte s = 0; s < 7; s++) {
          digitalWrite(num[n].segment[s], LOW);
        }
      }
      digitalWrite(batt1, LOW);
      digitalWrite(batt2, LOW);
      digitalWrite(decWrite, LOW);

      nextTime = micros() + 12000;
    }
    alternate = !alternate;
  }
}
