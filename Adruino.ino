


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  

#include <SPI.h>
#include <string.h>
//We define the SPI pìns
#define MAX6675_CS   10
#define MAX6675_SO   12
#define MAX6675_SCK  13
#define pi 3.14

//Pins
int PWM_pin = 9;


//Variables
float temperature_readfilter = 0.0;
float temperature_read = 0.0;
float temperature_read_ = 0.0;
float set_temperature = 100;
float d = 0.5;
int idx = 0;
int T1,T2,Tc,M;
float Kc,Ki,Kp,Kd,Ti,Td,uk=0,uk_1=0,t,k=0,Kii;
String temp_str;
float PHIm;
float alfa,b,c;
int temperature_1 = 70,ek=0,ek_1=0,ek_2=0, n = 0,m=0, Time,i = 0,j=0,power = 0,cnt = 0,wait=1;
int temperature_2 = 0;
String Kps,Kis,Kds;
float duty_cycle, OCRIA,kim;
int mode= 0, duong = 0;
char mode_str;
char a[100],a1[20];
String f = "";

void setup() {
  Serial.begin(9600);
  pinMode(PWM_pin,OUTPUT);
  TCCR2B = TCCR2B & B11111000 | 0x03;    // pin 3 and 1,K1 PWM frequency of 980.39 Hz
  Time = millis(); 
  lcd.init();
  lcd.backlight();

/*  TCCR1A=0; TCCR1B=0;
   // RESET lại 2 thanh ghi
  DDRB |= (1 << PB1);
  // Đầu ra PB1 là OUTPUT ( pin 9)
 
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM12)|(1 << WGM13);
    // chọn Fast PWM, chế độ chọn TOP_value tự do  ICR1
    TCCR1A |= (1 << COM1A1);
    // So sánh thường( none-inverting)
    ICR1 = 65535;
    // xung răng cưa tràn sau 65535 P_clock
    
    // Value=16838 -> độ rộng duty_cycle
    TCCR1B |= (1 << CS10)|(1 << CS12);
    // F_clock/64=16mhz/64=250 khz
    //F_pwm=250khz/65536=3.81469 hz*/
}




void loop() { 
  

  
    if (mode == 0){
  temperature_read = readThermocouple();
  delay(300);
  lcd.clear();
  lcd.setCursor(0,0);
  //lcd.print("PID TEMP control");
  lcd.print("PID TEMP control");
  lcd.setCursor(0,1);
  lcd.print("S:");
  lcd.setCursor(2,1);
  lcd.print(M,1);
  lcd.setCursor(9,1);
  lcd.print("R:");
  lcd.setCursor(11,1);
  lcd.print(temperature_read,1);
  if(Serial.available() >0)
 {
  
  Serial.readBytes(a,48);

//Serial.print(a);
//  a=Serial.read();
 //while(1);
  if(a[0] == 'd'){
    strncpy(a1, a + 1, 6);
    a1[6] = '\0';
    f = f + a1;
   set_temperature = f.toFloat() ;
 // Serial.print(set_temperature,3);
  f="";
  }
  
  if(a[7] == 'e'){
    strncpy(a1, a + 8, 1);
    a1[1] = '\0';
    f = f + a1;
    mode_str = f.toFloat() ;
 // Serial.print(mode_str,3);
   f = "";
 }
 
  if(a[9] == 'a'){
    strncpy(a1, a + 10, 10);
    a1[10] = '\0';
    f = f + a1;
   Kp = f.toFloat() ;
//  Serial.print(Kp,3);
   f = "";
 }
   if(a[20] == 'b'){
    strncpy(a1, a + 21, 10);
    a1[10] = '\0';
    f = f + a1;
   Kd = f.toFloat() ;
 // Serial.print(Kd,3);
   f = "";
 }
   if(a[31] == 'c'){
    strncpy(a1, a + 32, 10);
    a1[10] = '\0';
    f = f + a1;
   Kii = f.toFloat() ;
   Ki = Kii*0.000001;
 // Serial.print(Ki,3);
   f = "";
 }
 if(a[42] == 'g'){
    strncpy(a1, a + 43, 2);
    a1[2] = '\0';
    f = f + a1;
   alfa = f.toFloat() ;
 // Serial.print(Ki,3);
   f = "";
 }
 if(a[45] == 'h'){
    strncpy(a1, a + 46, 2);
    a1[2] = '\0';
    f = f + a1;
   PHIm = f.toFloat() ;
 // Serial.print(Ki,3);
 PHIm = PHIm*pi/180;
   f = "";
 }
 mode =1 ;
}
  
  }
   
 
/*
  if(k == 0){
  while(!Serial.available());
  k=k+1;
  }*/
while(mode == 1){


 ///Auto-tuning
 if( (mode_str == 1) || (mode_str == 3))
 { 
  /// cho lò mở ban đầu
 if(wait){
  if(n == 0){
    //analogWrite(PWM_pin,255);
   digitalWrite(PWM_pin,HIGH);
    n=n+1;
    m = 0;
  };
  
  /// chế độ ON OFF
  if((mode_str== 1)){
  while(idx <= 3){
    cnt=cnt+1;
    temperature_read = readThermocouple();
   temperature_readfilter = temperature_read; //*0.1518 + temperature_read*0.8482;
  if((temperature_readfilter = 0.7*set_temperature) ){
  
      delay(1000);
  
      temperature_read_ = readThermocouple();
  //// cắt 70 lần đầu
  if((temperature_read_ > 0.7*set_temperature + 1) && (idx==0)){
      digitalWrite(PWM_pin,LOW);
      idx=idx+1;
  };
  ///// cắt 70 lần 2
  if((temperature_read_ < 0.7*set_temperature - 1) && (idx==1)){
    digitalWrite(PWM_pin,HIGH);
    T1 = cnt;
    idx=idx+1;
    i=1;
  };
  ////// cắt 70 lần 3 
  if((temperature_read_ > 0.7*set_temperature + 1) && (idx==2)){
    digitalWrite(PWM_pin,LOW);
    idx=idx+1;
    i=0;
  };
  ///// cắt 70 lần cuối
  if((temperature_read_ < 0.7*set_temperature - 1) && (idx==3)){
          digitalWrite(PWM_pin,HIGH);
          T2 = cnt;
          idx=idx+2;
          i=0;
          n=0;
  };
}
//// tìm temp lớn nhất & nhỏ nhất
  if(idx >= 1){
     temperature_read_ = readThermocouple();
     temperature_1=min(temperature_1,temperature_read_);
  };
  if(idx >= 2){
     temperature_read_ = readThermocouple();
     temperature_2=max(temperature_2,temperature_read_);
  };
  ////// hiển thị trong bộ ON OFF
 temperature_read = readThermocouple();
  lcd.setCursor(0,0);
  lcd.print("ON OFF MODE            ");
  //lcd.print("PID TEMP control");
  lcd.setCursor(0,1);
  lcd.print("S:");
  lcd.setCursor(2,1);
  lcd.print(PHIm,3);
  lcd.setCursor(9,1);
  lcd.print("R:");
  lcd.setCursor(11,1);
  lcd.print(temperature_read,1);
  
  temp_str=String(temperature_read,4);
  while(temp_str.length()<8){
    temp_str="0"+temp_str;
  }
   temp_str="00"+temp_str;
 
   Serial.print(temp_str);
  }
  
 

      //tính toán các hằng số PID  
          Tc=(T2-T1);
          M  = temperature_2-70 + 70-temperature_1;
          Kc = 2*d/(pi*M/2);  
          Td = ( tan(PHIm) + sqrt(4/alfa + tan(PHIm)*tan(PHIm)) ) * Tc / (4*pi);
          Ti = alfa*Td;
          Kp = Kc*cos(PHIm);
    
    Kps=String(Kp,3);
  while (Kps.length()<8){
    Kps="0"+Kps;
  };
     Kps="20"+Kps;
 //  Serial.print(Kps);
 
    Ki = Kp/Ti;
    kim=Ki*1000000;
     Kis=String(kim,3);
      while (Kis.length()<8){
    Kis="0"+Kis;
  };
     Kis="21"+Kis;
   //Serial.print(Kis);
   
    Kd = Kp*Td;
     Kds=String(Kd,3);
      while(Kds.length()<8){
    Kds="0"+Kds;
  };
     Kds="22"+Kds;
   //Serial.print(Kds);

   ///// truyền lần lượt các hệ số PID lên GUI
    if(j == 0){
      Serial.print(Kds);
    delay(100);
    Serial.print(Kps);
    delay(100);
    Serial.print(Kis);
    delay(100);
     j = 1;
   }
   
  }
  if((mode_str== 1) || (mode_str== 3)){
    
  t=1;
   if(m == 0){
 digitalWrite(PWM_pin,HIGH);
 m++;
   }
   temp_str=String(temperature_read,4);
  if (temp_str.length()<8){
    temp_str="0"+temp_str;
  };
  temp_str="00"+temp_str;
  //Serial.print(temp_str);

  //delay(1000);
////// chế độ PID
 /* Kp = 0.02343; 
  Ki = 0.000002647; 
  Kd = 1.7823; */
 temperature_read = readThermocouple();
 ek = set_temperature - temperature_read;
 uk=uk_1+Kp*(ek-ek_1)+Ki*t/2*(ek+ek_1)+Kd/t*(ek-2*ek_1+ek_2);
 ek_2=ek_1;
 ek_1=ek;
 uk_1=uk;
 
 if(uk > 1){
  uk=1;
 };
 if(uk < 0){
  uk=0;
 };

  
 uk=sqrt(uk);
 if(uk > 1){
  uk=1;
 };
 
 b=uk*2000;
 digitalWrite(PWM_pin,HIGH);
 if(b >= 1000){
 delay(1000);
  Serial.print(temp_str);
  delay(b-1000);
 }
 else delay(b);
 c=2000-b;
 digitalWrite(PWM_pin,LOW);
 if(c > 1000){
  delay(1000-b);
  Serial.print(temp_str);
  delay(b);
  delay(c-1000);
 }
 if(c == 1000){
  delay(c);
  Serial.print(temp_str);
 }
 else delay(c);


  }
  ///// phần hiển thị LCD
  temperature_read = readThermocouple();
  
  lcd.setCursor(0,0);
  lcd.print("PID MODE                  ");
  // lcd.setCursor(5,0);
   //lcd.print(Kp,3);
   //lcd.setCursor(10,0);
    // lcd.print(Kd,3);
  lcd.setCursor(0,1);
  lcd.print("S:");
  lcd.setCursor(2,1);
  lcd.print(uk,1);
  lcd.setCursor(9,1);
  lcd.print("R:");
  lcd.setCursor(11,1);
  lcd.print(temperature_read,1);
 }
 }


 //onoff mode
 if(mode_str == 2)
 {
  while(1){
 if(n == 0){
    //analogWrite(PWM_pin,255);
    duty_cycle=1;
 OCR1A =round(duty_cycle*65535);
    n=n+1;
  };
  
    temperature_read = readThermocouple();
   temperature_readfilter = temperature_read; //*0.1518 + temperature_read*0.8482;
  if((temperature_readfilter = set_temperature) ){
  
      delay(1000);
  
      temperature_read_ = readThermocouple();
  //// cắt 70 lần đầu
  if((temperature_read_ > set_temperature + 1) && (idx==0)){
      digitalWrite(PWM_pin,LOW);
      idx=idx+1;
  };
  ///// cắt 70 lần 2
  if((temperature_read_ < set_temperature - 1) && (idx==1)){
    digitalWrite(PWM_pin,HIGH);
    idx=idx+1;
  };
  ////// cắt 70 lần 3 
  if((temperature_read_ > set_temperature + 1) && (idx==2)){
    digitalWrite(PWM_pin,LOW);
    idx=idx+1;
  };
  ///// cắt 70 lần cuối
  if((temperature_read_ < set_temperature - 1) && (idx==3)){
          digitalWrite(PWM_pin,HIGH);
          idx=0;
  };
  }
  
 
 delay(300);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ON OFF MODE           ");
  //lcd.print("PID TEMP control");
  lcd.setCursor(0,1);
  lcd.print("S:");
  lcd.setCursor(2,1);
  lcd.print(M,1);
  lcd.setCursor(9,1);
  lcd.print("R:");
  lcd.setCursor(11,1);
  lcd.print(temperature_read,1);
  
  temp_str=String(temperature_read,2);
  while(temp_str.length()<8){
    temp_str="0"+temp_str;
  }
   temp_str="00"+temp_str;
   Serial.print(temp_str);
   //delay(1000);
 }
 }



}

}



double readThermocouple() {

  uint16_t v;
  float filter=0;
  pinMode(MAX6675_CS, OUTPUT);
  pinMode(MAX6675_SO, INPUT);
  pinMode(MAX6675_SCK, OUTPUT);
  
  digitalWrite(MAX6675_CS, LOW);
  delay(1);

  v = shiftIn(MAX6675_SO, MAX6675_SCK, MSBFIRST);
  v <<= 8;
  v |= shiftIn(MAX6675_SO, MAX6675_SCK, MSBFIRST);
  
  digitalWrite(MAX6675_CS, HIGH);
  if (v & 0x4) 
  {    
    // Bit 2 indicates if the thermocouple is disconnected
    return NAN;     
  }

  // The lower three bits (0,1,2) are discarded status bits
  v >>= 3;
  // The remaining bits are the number of 0.25 degree (C) counts
  return 0.25*v;
}
