#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void setup() {
  Serial.begin(115200);
  sbi(ADCSRA, ADPS2);
  cbi(ADCSRA, ADPS1);
  cbi(ADCSRA, ADPS0);
}
int pins[] = {A6,A7,A0,A1,A2,A3};

long ti = 0;

void loop() {
  long t = micros();
  if (digitalRead(2) == 1) {
    while(micros() - t < ti*32);
    Serial.print(analogRead(A6));
    Serial.print(",");
    Serial.print(analogRead(A7));
    Serial.print(",");
    Serial.print(analogRead(A0));
    Serial.print(",");
    Serial.print(analogRead(A1));
    Serial.print(",");
    Serial.print(analogRead(A2));
    Serial.print(",");
    Serial.print(analogRead(A3));
    Serial.print(",");
    Serial.println(ti);
    ti++;
    while(digitalRead(2) == 1);
    //_delay_us(1000);
  }
//  Serial.print(digitalRead(2)*1000);
//  Serial.print(",");
//  Serial.print(analogRead(A6)+1500);
//  Serial.print(",");
//  Serial.print(analogRead(A7)+3000);
//  Serial.print(",");
//  Serial.print(analogRead(A0)+4500);
//  Serial.print(",");
//  Serial.print(analogRead(A1)+6000);
//  Serial.print(",");
//  Serial.print(analogRead(A2)+7500);
//  Serial.print(",");
//  Serial.println(analogRead(A3)+9000);
}
