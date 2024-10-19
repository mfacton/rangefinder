#include "I2Cdev.h"
#include "LiquidCrystal.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWorld;
VectorFloat gravity;
float euler[3];
float ypr[3];

uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24;
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  Serial.begin(115200);
  while (!Serial);
  mpu.initialize();
  devStatus = mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  //532  838 1229  37  8 -13
  mpu.setXAccelOffset(532);
  mpu.setYAccelOffset(838);
  mpu.setZAccelOffset(1229);
  mpu.setXGyroOffset(37);
  mpu.setYGyroOffset(8);
  mpu.setZGyroOffset(-13);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;

    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    Serial.print(F("DMP Initialization failed"));
  }
  Serial.println(F("Initialized"));
  mySetup();
}


void loop() {
  if (!dmpReady) return;
  while (!mpuInterrupt && fifoCount < packetSize) {
    myLoop();
  }
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  fifoCount = mpu.getFIFOCount();

  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
    Serial.println(F("FIFO overflow!"));

  } else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    mpu.getFIFOBytes(fifoBuffer, packetSize);

    fifoCount -= packetSize;

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  }
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
byte b1 = 5;
byte b2 = 4;
byte refPin = 44;
byte decRead = 48;
byte decWrite = 25;
byte batt1 = 27;
byte batt2 = 26;
byte pointer = 29;
byte crossRead = 49;
byte crossWrite = 28;

bool lastRef = false;

bool readPhase = false;

bool displayDecimal = false;
bool displayBatt1 = false;
bool displayBatt2 = false;
bool displayCross = false;

bool battFlipTime;

long b2Start;
long lastTimeb2 = 0;
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

double calculateDist (double angle1, double  angle2, double dist1, double dist2) {
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

void b2Held() {
  if (!b2Hold) {
    displayCross = false;
    res = calculateDist(a1, a2, d1, d2);
    String str = ((String)res).substring(0, 4);
    str.replace(".", "");
    if (res < 10) {
      displayDecimal = true;
      setDigit(0, 11);
      setDigit(1, str.substring(0, 1).toInt());
      setDigit(2, str.substring(1, 2).toInt());
    } else {
      setDigit(0, str.substring(0, 1).toInt());
      setDigit(1, str.substring(1, 2).toInt());
      setDigit(2, str.substring(2, 3).toInt());
      if (res >= 100) {
        displayDecimal = false;
      } else {
        displayDecimal = true;
      }
    }
    battFlipTime = millis() + 500;
    b2Hold = true;
  }
  if (millis() > battFlipTime) {
    displayBatt1 ^= true;
    displayBatt2 ^= true;
    battFlipTime = millis() + 500;
  }
}

void b2Released() {
  if (millis() - b2Start <= 1000) {
    if (!distToggle) {
      d1 = lastRead;
      a1 = ypr[0];
      displayBatt1 = true;
      displayBatt2 = false;
      distToggle = true;
    } else {
      d2 = lastRead;
      a2 = ypr[0];
      displayBatt2 = true;
      distToggle = false;
    }
  } else {
    displayBatt1 = false;
    displayBatt2 = false;
    b2Hold = false;
  }
  readScreen = true;
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

void mySetup() {
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
  pinMode(crossRead,INPUT);
  pinMode(crossWrite,OUTPUT);
  pinMode(pointer, OUTPUT);

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
  screen[0] = 11;
  screen[1] = 11;
  screen[2] = 11;
}

void readSegment(byte num, byte segment) {
  reads[num].segment[segment] = digitalRead(inPinNum[num].segment[segment]);
}

void myLoop () {
  if (readScreen) {
    if (digitalRead(refPin) == HIGH) {
      if (!lastRef) {
        lastRef = true;
      }
    } else {
      if (lastRef) {
        if (!readPhase) {
          delayMicroseconds(1200);
          readSegment(2,2);
          readSegment(2,3);
          readSegment(2,4);
          displayDecimal = digitalRead(decRead);
          displayCross = digitalRead(crossRead);
          readSegment(1,2);
          readSegment(1,3);
          readSegment(1,4);
          readSegment(0,2);
          readSegment(0,3);
          readPhase = true;
        }else{
          delayMicroseconds(1900);
          readSegment(0,4);
          readSegment(0,5);
          readSegment(0,0);
          readSegment(0,1);
          readSegment(1,5);
          readSegment(1,0);
          readSegment(1,1);
          readSegment(2,5);
          readSegment(2,0);
          readSegment(2,1);
          for (byte n = 0;n < 3;n++) {
            screen[n] = checkNum(n);
          }
          readPhase = false;
        }
        lastRef = false;
      }
    }
  }
  /*
  Serial.print("'");
  Serial.print(screen[0]);
  Serial.print(",");
  Serial.print(screen[1]);
  Serial.print(",");
  Serial.print(screen[2]);
  Serial.println("'");
  */
  
  validate();
  /*
  if (digitalRead(b2)) {
    if (!lastb2) {
      b2Start = millis();
      readScreen = false;
      readPhase = false;
      lastb2 = true;
    }
    if (millis() - b2Start > 1000) {
      b2Held();
    }else{
      nonAction();
    }
  } else {
    if (lastb2) {
      Serial.println("Released");
      b2Released();
      readScreen = true;
      lastb2 = false;
    }else{
      nonAction();
    }
  }
  */
  if (lastb2 && millis() - b2Start > 1000) {
    b2Held();
  }else{
    setDigit(0, screen[0]);
    setDigit(1, screen[1]);
    setDigit(2, screen[2]);
  }
  if (digitalRead(4)) {
    if (!lastb2) {
      lastb2 = true;
      b2Start = millis();
      readScreen = false;
      readPhase = false;
    }
    lastTimeb2 = millis();
  }
  if (lastb2 && millis() - lastTimeb2 > 200) {
    b2Released();
    lastb2 = false;
  }
  
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
      digitalWrite(crossWrite, displayCross);
      digitalWrite(pointer, HIGH);

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
      digitalWrite(crossWrite, LOW);
      digitalWrite(pointer, LOW);
      
      nextTime = micros() + 10000;
    }
    alternate = !alternate;
  }
}
