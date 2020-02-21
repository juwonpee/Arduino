/* 본 스케치는 arduino.cc의 JoghChi님이 작성하신 스케치입니다.*/

#include<Wire.h>

const int MPU=0x68;  //MPU 6050 의 I2C 기본 주소
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int loopcount;

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

void setup(){
  delay(5000);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("lcd pass");
  delay(3000);
  
  Serial.begin(9600);
  Serial.println("serial pass");
  lcd.setCursor(0,0);
  lcd.print("serial pass");
  delay(3000);


  lcd.setCursor(0,0);
  lcd.print("MPU setup start");
  delay(1000);
  Wire.begin();      //Wire 라이브러리 초기화
  lcd.setCursor(0,0);
  lcd.print("Wire.begin");
  Wire.beginTransmission(MPU); //MPU로 데이터 전송 시작
  lcd.print("Wire.begintransmission");
  lcd.setCursor(0,0);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  lcd.setCursor(0,0);
  lcd.print("Wire.write(0x68");
  Wire.write(0);     //MPU-6050 시작 모드로
  lcd.setCursor(0,0);
  lcd.print("Wire.write(0)");
  Wire.endTransmission(true); 
  lcd.setCursor(0,0);
  lcd.print("Wire.endtransmission");
  Serial.print("MCU setup pass");
  lcd.setCursor(0,0);
  lcd.print("MCU setup pass");
  delay(3000);
}

void loop(){
  loopcount = loopcount + 1;
  Serial.print("pass");
  Serial.println(loopcount);
  lcd.setCursor(0,0);
  lcd.println("pass count");
  lcd.setCursor(11,0);
  lcd.print(loopcount);
  
  Wire.beginTransmission(MPU);    //데이터 전송시작
  Wire.write(0x3B);               // register 0x3B (ACCEL_XOUT_H), 큐에 데이터 기록
  Wire.endTransmission(false);    //연결유지
  Wire.requestFrom(MPU,14,true);  //MPU에 데이터 요청
  //데이터 한 바이트 씩 읽어서 반환
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  //시리얼 모니터에 출력
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  delay(333);
  
  //output to lcd display
  lcd.setCursor(0, 1);
  lcd.print("AX"); 
  lcd.setCursor(4, 1);
  lcd.print(AcX);
}
