#include <Wire.h>
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define COM1 2
#define calibration_samples 1000
#define calibration_increment 4000
#define LED1 8
#define LED2 9
#define LED3 10
#define BTN0 4
#define BTN1 5
#define BTN2 6
#define BTN3 7

float gyro, cal, angle;
long loop_timer;

const uint8_t digitPins[3][7][2] = {{{3,1},{2,0},{1,0},{0,0},{0,1},{2,1},{1,1}},{{3,3},{2,2},{1,2},{0,2},{0,3},{2,3},{1,3}},{{3,5},{2,4},{1,4},{0,4},{0,5},{2,5},{1,5}}};

const uint8_t digits[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};

uint8_t pinNums[6] = {A3, A2, A1, A0, A7, A6};
uint8_t pinsG[4];
short lastDist = -1;
short dist = -1;

void setup() {
  Serial.begin(115200);
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, 1);

  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(BTN3, INPUT);

  //before to allow 2560 to boot before clear
  setupMPU();

  for (byte i = 0; i < 10; i++) {
    sendClear();
    delay(10);
  }
  digitalWrite(LED1, 0);
  sendSegment(0,6,1);
  sendSegment(3,6,1);
  sendSegment(1,15,1);
}

uint8_t getDigit(uint8_t digit) {
  for (uint8_t n = 0; n < 11; n++) {
    bool valid = true;
    for (uint8_t s = 0; s < 7; s++) {
      if (((digits[n]>>s)&1) != (pinsG[digitPins[digit][s][0]]>>digitPins[digit][s][1]&1)) {
        valid = false;
        break;
      }
    }
    if (valid) {
      return n%10;
    }
  }
  return -1;
}

uint8_t r (uint8_t pin) {
  uint16_t num = analogRead(pin);
  if (num < 5) {
    return 0;
  } else if (num > 1020) {
    return 2;
  }
  return 1;
}

long downTime = -1;
bool lastB1 = false;
bool lastB2 = false;
bool lastB3 = false;

uint16_t dist0;
uint16_t dist1;
float ang0;
float ang1;
bool measure = 0;

byte count = 0;
bool dispAng = true;

void loop() {
  if (dispAng) {
    count++;
  }else{
    count = 0;
  }
  if (count == 255) {
    float ang = regAng(angle);
    bool sub = false;
    while (ang > 100) {
      ang -= 100;
      sub = true;
    }
    sendNum(0,(byte)ang);
    sendSegment(0,15,sub);
    count = 0; 
  }
  
  if (downTime != -1) {
    if (millis()-downTime >= 600) {
      downTime = -1;
      pinMode(BTN0, INPUT);
    }
  }

  bool b1 = digitalRead(BTN1);
  bool b2 = digitalRead(BTN2);
  bool b3 = digitalRead(BTN3);

  if (b1 && !lastB1) {
    dispAng = true;
    downTime = millis();
    pinMode(BTN0, OUTPUT);
    dist = -1;
  }

  if (b2 && !lastB2) {
    dispAng = true;
    if (!measure) {
      //0
      dist0 = dist;
      ang0 = angle;
      sendNum(2, dist);
      sendSegment(6,7,1);
      //Serial.println("save 0");
    }else{
      //1
      dist1 = dist;
      ang1 = angle;
      sendNum(3, dist);
      sendSegment(7,8,1);
      //Serial.println("save 1");
    }
    measure = !measure;
  }
  if (b3 && !lastB3) {
    dispAng = false;
    ang0 = regAng(ang0);
    ang1 = regAng(ang1);

    if (abs(ang1-ang0) > 180) {
      if (ang1 > ang0) {
        ang0 += 360;
      }else{
        ang1 += 360;
      }
    }

    float diff = abs(ang1-ang0);
    if (diff < 100) {
      sendNum(0,(byte)diff);
      sendSegment(0,15,0);
    }else{
      sendNum(0,((byte)diff)-100);
      sendSegment(0,15,1);
    }
    float calculated = sqrt(dist0*dist0+dist1*dist1-2*dist0*dist1*cos(diff*PI/180));
    calculated *= 10;
    sendNum(1, (uint16_t)round(calculated));
    sendSegment(7,3,1);
    //Serial.println("DATA:");
    //Serial.println(dist0);
    //Serial.println(ang0);
    //Serial.println(dist1);
    //Serial.println(ang1);
    sendSegment(0,4,1);
    sendSegment(7,14,1);
  }

  lastB1 = b1;
  lastB2 = b2;
  lastB3 = b3;

  if (dist != lastDist && dist != -1) {
    lastDist = dist;
    sendNum(1, dist);
    sendSegment(7,0,1);
  }
  
  bool data = false;
  uint8_t pins[4] = {0x00,0x00,0x00,0x00};
  long ts = micros();
  if (digitalRead(COM1) == 1) {
    for (uint8_t i = 0; i < 4; i++) {
      while (micros()-ts < i*2000+1000);
      for (uint8_t p = 0; p < 6; p++) {
        if (analogRead(pinNums[p]) < 50) {
          pins[3 - i] |= (1<<p);
          data = true;
        }
      }
      while (micros()-ts < (i+1)*2000);
    }
  }
  if (data) {
    for (uint8_t c = 0; c < 4; c++) {
      pinsG[c] = pins[c];
    }
    uint8_t d2 = getDigit(2);
    uint8_t d1 = getDigit(1);
    uint8_t d0 = getDigit(0);

    if (!(d0 == 255 || d1 == 255 || d2 == 255)) {
      lastDist = dist;
      dist = d2 * 100 + d1 * 10 + d0;
    }
  }
  loopMPU();
}

float regAng(float ang) {
  while (ang > 360) {
    ang -= 360;
  }
  while (ang < 0) {
    ang += 360;
  }
  return ang;
}

// send format
  // number:
  // r = row
  // x = number
  // 0b0rrxxxx xxxxxxx
  // led:
  // c = com
  // p = pin
  // s = state
  // 0b100000s cccpppp

void sendClear() {
  Serial.write(0x7F);
  Serial.write(0xFF);
}

void sendNum(uint8_t row, uint16_t num) {
  sendData(row<<11 | num);
}

void sendSegment(uint8_t com, uint8_t pin, bool state) {
  sendData((1<<13) | (state<<7) | (com<<4) | pin);
}

void sendData(uint16_t data) {
  //sending 14 bit data because of issues with proper order
  //0xxxxxxx 1xxxxxxx format
  //  uint8_t a = data>>7;
  //  uint8_t b = (1<<7) | (data&0x7F);
  Serial.write(data>>7);//left
  Serial.write((1<<7) | (data&0x7F));//right
}

void setupMPU() {
  Wire.begin();
  setupRegisters();
  calibrate();
  loopTimer();
}

void readRegisters() {
  Wire.beginTransmission(0x68);
  Wire.write(0x47);
  Wire.endTransmission();
  Wire.requestFrom(0x68,2);
  while(Wire.available() < 2);
  gyro = Wire.read()<<8|Wire.read();
}

void loopMPU() {
  readRegisters();
  gyro -= cal;
  long ct = micros();
  angle += gyro * 0.000001 * (ct-loop_timer) / 65.5;
  loop_timer = ct;
}

void sen (uint8_t address, uint8_t data) {
  Wire.beginTransmission(0x68);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

void setupRegisters () {
  sen(0x6B, 0x08);
  sen(0x1B, 0x08);
  sen(0x1C, 0x10);
}

void loopTimer () {
  loop_timer = micros();
}

void calibrate() {
  long ztot = 0;
  loopTimer();
  for (uint16_t ci = 0; ci < calibration_samples; ci++){
    readRegisters();
    ztot += gyro;
    while(micros() - loop_timer < calibration_increment);
    loopTimer();
  }
  cal = ztot/((float)calibration_samples);
}
