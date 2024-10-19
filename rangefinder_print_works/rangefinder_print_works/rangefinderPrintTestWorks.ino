const uint8_t coms[8] = {29,28,27,26,A3,A2,A1,A0};
const uint8_t pins[16] = {25,24,23,22,A15,A14,A13,A12,A11,A10,A9,A8,A7,A6,A5,A4};

//const uint8_t extra[26] = {0x50,0x60,0x70,0x01,0x71,0x73,0x04,0x75,0x06,0x16,0x26,0x36,0x47,0x57,0x67,0x77,0x78,0x3A,0x7A,0x0E,0x7E,0x1F,0x2F,0x3F,0x7F};
/* XS
 * X/
 * XY
 * speed
 * XM
 * decimal
 * degree
 * XK/XH
 * H
 * Q
 * K
 * I
 * level
 * high
 * YY
 * lightning
 * YM
 * ZY
 * flag
 * ZM
 * scan
 * angle
 * R
 * P
 * J
 * battery
 */

//const uint8_t digits[11] = {0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b01111111,0b01101111,0b00000000};
const uint8_t digits[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
const uint8_t digitPins[4][4][7] = {{{0x03,0x11,0x21,0x31,0x33,0x13,0x23},
                                    {0x05,0x14,0x24,0x34,0x35,0x15,0x25},
                                    {0x80,0x80,0x80,0x80,0x80,0x80,0x80},
                                    {0x80,0x80,0x80,0x80,0x80,0x80,0x80}},
                                    {{0x72,0x61,0x51,0x41,0x42,0x62,0x52},
                                    {0x74,0x63,0x53,0x43,0x44,0x64,0x54},
                                    {0x76,0x65,0x55,0x45,0x46,0x66,0x56},
                                    {0x07,0x1E,0x2E,0x3E,0x37,0x17,0x27}},
                                    {{0x39,0x28,0x18,0x08,0x09,0x29,0x19},
                                    {0x3B,0x2A,0x1A,0x0A,0x0B,0x2B,0x1B},
                                    {0x3D,0x2C,0x1C,0x0C,0x0D,0x2D,0x1D},
                                    {0x80,0x80,0x80,0x80,0x80,0x80,0x80}},
                                    {{0x79,0x68,0x58,0x48,0x49,0x69,0x59},
                                    {0x7B,0x6A,0x5A,0x4A,0x4B,0x6B,0x5B},
                                    {0x7D,0x6C,0x5C,0x4C,0x4D,0x6D,0x5D},
                                    {0x80,0x80,0x80,0x80,0x80,0x80,0x80}}};
uint16_t segments[8];

void setDigit(uint8_t row, uint8_t place, uint8_t digit_) {
  uint8_t digit = digits[digit_];
  if (digitPins[row][place][0]>>7 & 1) {
    return;
  }
  for (uint8_t i = 0; i < 7; i++) {
    if (digit>>i & 1) {
      segments[digitPins[row][place][i]>>4] |= 1<<(digitPins[row][place][i]&0xF);
    }else{
      segments[digitPins[row][place][i]>>4] &= 0xFFFF ^ (1<<(digitPins[row][place][i]&0xF));
    }
  }
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 1) {
    uint8_t a = Serial.read();//left
    if (a>>7) {
      return;
    }
    
    uint8_t b = Serial.read();//right

    if (a == 0x7F && b == 0xFF) {//clear
      segments[0] = 0;
      segments[1] = 0;
      segments[2] = 0;
      segments[3] = 0;
      segments[4] = 0;
      segments[5] = 0;
      segments[6] = 0;
      segments[7] = 0;
      return;
    }
    
    uint16_t data = (a<<7)|(b&0x7F);
    if (data>>13 == 1) {
      //led
      bool state = (data>>7)&1;
      uint8_t pos = data&0x7F;
      if (state) {
        segments[pos>>4] |= 1<<(pos&0xF);
      }else{
        segments[pos>>4] &= 0xFFFF^(1<<(pos&0xF));
      }
    }else{
      //number
      uint8_t row = data>>11;
      uint16_t num = data&0x7FF;
      byte nums[4];
      //0 is right
      //3 is left
      for (byte x = 0; x < 4; x++) {
        nums[x] = num%10;
        num/=10;
      }
      if (nums[3] == 0) {
        nums[3] = 10;
      }
      if (nums[2] == 0 && nums[3] == 10) {
        nums[2] = 10;
      }
      //7 3
      if (!((segments[7]>>3)&1) && nums[1] == 0 && nums[2] == 10) {
        nums[1] = 10;
      }
      
      setDigit(row,0,nums[0]);
      setDigit(row,1,nums[1]);
      setDigit(row,2,nums[2]);
      setDigit(row,3,nums[3]);
    }    
  }
  for (byte i = 0; i < 8; i++) {
    setComs(1, i);
    setPins(2, i);
    delay(1);
    setComs(2, i);
    setPins(1, i);
    delay(1);
  }
}

void setPins(byte off_state, byte com) {
  for (byte p = 0; p < 16; p++) {
    if ((segments[com]>>p)&1==1) {
      setWire(pins[p], off_state ^ 2);
    }else{
      setWire(pins[p], off_state);
    }
  }
}

void setComs(byte off_state, byte active) {
  for (byte i = 0; i < 8; i++) {
    if (i == active) {
      setWire(coms[i], off_state ^ 2);
    }else{
      setWire(coms[i], off_state);
    }
  }
}

void setWire(byte wire, byte state) {
  if (state == 0) {
    pinMode(wire, OUTPUT);
    digitalWrite(wire, LOW);
  }else if (state == 1) {
    pinMode(wire, INPUT);
  }else if (state == 2) {
    pinMode(wire, INPUT_PULLUP);
  }else{
    pinMode(wire, OUTPUT);
    digitalWrite(wire, HIGH);
  }
}
