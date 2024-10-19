//segments chart
//A = 0
//B = 1
//C = 2
//D = 3
//E = 4
//G = 5

//cases int chart
//Nothing = 10
//Dash = 11
boolean decimalSignal;
boolean decimalDisplay;
int holdNum;
String screen = "";
char screenList[5];

typedef struct {
  unsigned int segments[6];
} digitPins;

typedef struct {
  boolean segments[6];
} digitCases;

typedef struct {
  boolean segments[6];
} digitStates;

digitCases digitCase[12];
digitPins digitPin[3];
digitStates digitState[3];

void initDigitPin (byte d, unsigned int A, unsigned int B, unsigned int C, unsigned int D, unsigned int E, unsigned int G) {
  digitPin[d].segments[0] = A;
  digitPin[d].segments[1] = B;
  digitPin[d].segments[2] = C;
  digitPin[d].segments[3] = D;
  digitPin[d].segments[4] = E;
  digitPin[d].segments[5] = G;
}

void initDigitCase (byte d, bool A, bool B, bool C, bool D, bool E, bool G) {
  digitCase[d].segments[0] = A;
  digitCase[d].segments[1] = B;
  digitCase[d].segments[2] = C;
  digitCase[d].segments[3] = D;
  digitCase[d].segments[4] = E;
  digitCase[d].segments[5] = G;
}

void setup() {
  Serial.begin(115200);
  //initialize input pins
  initDigitPin(0, 10, 9, A0, 2, 12, 11);
  initDigitPin(1, 7, 6, A3, A2, A1, 8);
  initDigitPin(2, 4, 3, A7, A6, A5, 5);

  //initialize possible cases
  initDigitCase(0, 1, 1, 1, 1, 1, 0);
  initDigitCase(1, 0, 1, 1, 0, 0, 0);
  initDigitCase(2, 1, 1, 0, 1, 1, 1);
  initDigitCase(3, 1, 1, 1, 1, 0, 1);
  initDigitCase(4, 0, 1, 1, 0, 0, 1);
  initDigitCase(5, 1, 0, 1, 1, 0, 1);
  initDigitCase(6, 1, 0, 1, 1, 1, 1);
  initDigitCase(7, 1, 1, 1, 0, 0, 0);
  initDigitCase(8, 1, 1, 1, 1, 1, 1);
  initDigitCase(9, 1, 1, 1, 1, 0, 1); //make sure of how 9 looks
  initDigitCase(10, 0, 0, 0, 0, 0, 0);
  initDigitCase(11, 0, 0, 0, 0, 0, 1);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 6; j++) {
      pinMode(digitPin[i].segments[j], INPUT);
    }
  }
  delay(5000);
}

boolean checkDigit (int digit, int num, boolean flip) {
  if (!flip) {
    for (int i = 0; i < 6; i++) {
      if (digitState[digit].segments[i] != digitCase[num].segments[i]) {
        return false;
      }
    }
  } else {
    for (int i = 0; i < 6; i++) {
      if (digitState[digit].segments[i] == digitCase[num].segments[i]) {
        return false;
      }
    }
  }
  return true;
}

int checkDigit2 (int digit, boolean flip) {
  for (int n = 0; n < 12; n++) {
    if (checkDigit(digit, n, flip)) {
      return n;
    }
  }
  return -1;
}

int parseNumbers(boolean flip) {
  for (int d = 0; d < 3; d++) {
    holdNum = checkDigit2(d, flip);
    if (holdNum == -1) {
      return false;
    }
    if (holdNum == 10) {
      screen += " ";
    } else if (holdNum == 11) {
      screen += "-";
    } else {
      screen += holdNum;
    }
  }
  return true;
}

void loop() {
  if (digitalRead(11) == 1) {
    while(digitalRead(11) == 1);
    delay(1);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 6; j++) {
        if (digitPin[i].segments[j] < 14) {
          digitState[i].segments[j] = digitalRead(digitPin[i].segments[j]);
        } else {
          if (analogRead(digitPin[i].segments[j]) > 650) {
            digitState[i].segments[j] = true;
          } else {
            digitState[i].segments[j] = false;
          }
        }
      }
    }
    if (analogRead(A4) > 500) {
      decimalSignal = true;
    }else{
      decimalSignal = false;
    }
    screen = "";
    decimalDisplay = decimalSignal;
    if (!parseNumbers(false)) {
      decimalDisplay = !decimalSignal;
      screen = "";
      !parseNumbers(true); 
    }
    if (screen != "888" && screen != "000" && screen != "---" && screen != "   ") {
      screen.toCharArray(screenList, 5);
      Serial.print(screenList[0]);
      Serial.print(screenList[1]);
      if (decimalDisplay) {
        Serial.print(".");
      }
      Serial.print(screenList[2]);
      Serial.print("x");
      screen = "";
    }
  }
}
