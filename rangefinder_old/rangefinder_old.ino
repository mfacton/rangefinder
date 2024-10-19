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
  rSetup();
  dSetup();
}


void loop() {
  if (!dmpReady) return;
  while (!mpuInterrupt && fifoCount < packetSize) {
    lLoop();
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
//display -------------------------------------------------------------------------------------------------------------------------------
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

void dSetup() {
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

void dLoop () {
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
      nextTime = micros() + 3000;
    } else {
      for (byte n = 0; n < 3; n++) {
        for (byte s = 0; s < 7; s++) {
          digitalWrite(num[n].segment[s], LOW);
        }
      }
      nextTime = micros() + 12000;
    }
    alternate = !alternate;
  }
}
//logic ------------------------------------------------------------------------------------------------------------------------------------------------
byte b1 = 0;
byte b2 = 0;

double d1;
double d2;
byte a1;
byte a2;
double res;

bool first = false;

bool phase = false;
String d;

long startPress;
bool lastPress;

void lSetup() {
  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
}

double calculateDist (double angle1, double angle2, double dist1, double dist2) {
  return sqrt(pow(dist1, 2) + pow(dist2, 2) - 2 * dist1 * dist2 * cos(M_PI*abs(angle1 - angle2)/180));
}

void lLoop() {
  dLoop();
  rLoop();
  if (digitalRead(b2)) {
    if (millis() - startPress > 1000) {
      if (!first) {
        res = calculateDist(a1, a2, d1, d2);
      }
    }
    if (!lastPress) {
      startPress = millis();
    }
    lastPress = true;
  } else {
    if (lastPress && millis() - startPress <= 1000) {
      if (!phase) {
        d1 = lastValid;
        a1 = ypr[0];
        phase = true;
      } else {
        d2 = lastValid;
        a2 = ypr[0];
        phase = false;
      }
    }
    lastPress = false;
  }
}
//recognition----------------------------------------------------------------------------------------------------------------------------------
struct inPins {
  byte segment[6];
};

struct pinReads {
  byte segment[6];
};

byte refPin = 0;
byte decPin = 0;
bool flip;
bool decimal;

pinReads reads[3];
inPins inPinNum[3];
byte screen[3];
double lastValid = -1;

void rSetup() {
  inPinNum[0].segment[0] = 0;
  inPinNum[0].segment[1] = 0;
  inPinNum[0].segment[2] = 0;
  inPinNum[0].segment[3] = 0;
  inPinNum[0].segment[4] = 0;
  inPinNum[0].segment[5] = 0;

  inPinNum[1].segment[0] = 0;
  inPinNum[1].segment[1] = 0;
  inPinNum[1].segment[2] = 0;
  inPinNum[1].segment[3] = 0;
  inPinNum[1].segment[4] = 0;
  inPinNum[1].segment[5] = 0;

  inPinNum[2].segment[0] = 0;
  inPinNum[2].segment[1] = 0;
  inPinNum[2].segment[2] = 0;
  inPinNum[2].segment[3] = 0;
  inPinNum[2].segment[4] = 0;
  inPinNum[2].segment[5] = 0;
  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 6; s++) {
      pinMode(inPinNum[n].segment[s], INPUT);
    }
  }
  pinMode(refPin, INPUT);
}

void validate() {
  if (screen[1] == 11 || screen[2] == 11 || screen[0] == 10) {
    return;
  }
  if (decimal) {
    lastValid = ((String)screen[0] + (String)screen[0] + "." + (String)screen[0]).toDouble();
  }else{
    lastValid = ((String)screen[0] + (String)screen[0] + (String)screen[0]).toDouble();
  }
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
  Serial.println("error during recognition");
}

void rLoop() {
  flip = digitalRead(refPin);
  for (byte n = 0; n < 3; n++) {
    for (byte s = 0; s < 6; s++) {
      reads[n].segment[s] = digitalRead(inPinNum[n].segment[s]) ^ flip;
    }
  }
  decimal = digitalRead(decPin) ^ flip;
  for (byte n = 0; n < 3; n++) {
    screen[n] = checkNum(n);
  }
  validate();
}
