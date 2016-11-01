#include <Wire.h>

#define VCNL4000_ADDRESS 0x13  // 0x26 write, 0x27 read

#include <Servo.h> 
#define BLUE 10
#define RED 8
#define BUTTON 5
#define SWITCH 4
#define SWITCH2 3
#define LEFT 780//850-986
#define RIGHT 900//590-940

 
Servo servoR;  // create servo object to control a servo 
Servo servoL;  // create servo object to control a second servo 

int speedL;
int speedR;

  uint16_t Lsen;    // variable to read the value from the analog pin 
  uint16_t Rsen;    // variable to read the value from the analog pin
  uint8_t state = 0;
  uint16_t buttonState;
  uint16_t switchState;
  uint16_t switch2State;


// VCNL4000 Register Map
#define COMMAND_0 0x80  // starts measurments, relays data ready info
#define PRODUCT_ID 0x81  // product ID/revision ID, should read 0x11
#define IR_CURRENT 0x83  // sets IR current in steps of 10mA 0-200mA
#define AMBIENT_PARAMETER 0x84  // Configures ambient light measures
#define AMBIENT_RESULT_MSB 0x85  // high byte of ambient light measure
#define AMBIENT_RESULT_LSB 0x86  // low byte of ambient light measure
#define PROXIMITY_RESULT_MSB 0x87  // High byte of proximity measure
#define PROXIMITY_RESULT_LSB 0x88  // low byte of proximity measure
#define PROXIMITY_FREQ 0x89  // Proximity IR test signal freq, 0-3
#define PROXIMITY_MOD 0x8A  // proximity modulator timing

int ambientValue, proximityValue;

void setup(){
  Serial.begin(9600);
  
  servoR.attach(7);  // attaches the servo on pin 9 to the servo object
  servoL.attach(6);  // attaches the servo on pin 9 to the servo object
  pinMode(A2, INPUT); //left sensor
  pinMode(A1, INPUT); //right sensor
  pinMode(RED, OUTPUT); //red led
  pinMode(BLUE, OUTPUT); // blue led
  pinMode(BUTTON, INPUT_PULLUP); //button
  //Enable pullup  
  //digitalWrite(BUTTON, HIGH); 
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(SWITCH2, INPUT_PULLUP); 
  //Enable pullup  
 // digitalWrite(SWITCH, HIGH); 
  
  Wire.begin();  // initialize I2C stuff
  
  /* Test that the device is working correctly */
  byte temp = readByte(PRODUCT_ID);
  if (temp != 0x11)  // Product ID Should be 0x11
  {
    Serial.print("Something's wrong. Not reading correct ID: 0x");
    Serial.println(temp, HEX);
  }
  else
    Serial.println("VNCL4000 Online...");
  
  /* Now some VNCL400 initialization stuff*/
  writeByte(AMBIENT_PARAMETER, 0x0F);  // Single conversion mode, 128 averages
  writeByte(IR_CURRENT, 20);  // Set IR current to 200mA
  writeByte(PROXIMITY_FREQ, 2);  // 781.25 kHz
  writeByte(PROXIMITY_MOD, 0x81);  // 129, recommended by Vishay
}

int counter = 0;
int Count = 0;
int BigCount = 0;

void loop(){
  buttonState = digitalRead(BUTTON);
  switchState = digitalRead(SWITCH);
  switch2State = digitalRead(SWITCH2);
  Lsen = analogRead(A2);           //read the values from the sensor
  Rsen = analogRead(A1);           //read the values from the sensor
  Serial.print("\nleft sensor:  ");
  Serial.print(Lsen);
    proximityValue = readProximity();
    if(buttonState == LOW)
      state = 1;
    if(proximityValue < 2380 && state == 2 && counter == 0){
      //state =5;
      state = 1;
    }
    if(Rsen > RIGHT && state == 2 && counter > 50){
      state = 5;
      counter = 0;
    }
    
    if(state == 0){
      servoL.write(90);    //90 stoped               // sets the servo position according to the scaled value 
      servoR.write(93);    //93 stoped               // sets the servo position according to the scaled value  
    }
  if(state == 1){
      if((Rsen < RIGHT) && (Lsen > LEFT)){ // go forward
        digitalWrite(RED, HIGH);
        digitalWrite(BLUE, HIGH);
        servoL.write(60);    //90 stoped 0 is forward    //60   //45
        servoR.write(138);    //95 stoped                //138+  //153
        Count = 0;
        delay(75);
      }
      else if((Rsen > RIGHT) && (Lsen > LEFT)){//turn right
        
        Count +=1;
        BigCount=0;
        digitalWrite(BLUE, HIGH);
        digitalWrite(RED, LOW);
        servoL.write(80);    //90 stoped 0 is forward     //80  //45
        servoR.write(83);    //95 stoped                  //83  //138
        delay(75);
      }
      else if((Rsen < RIGHT) && (Lsen < LEFT)){ // turn left
        digitalWrite(RED, HIGH);
        digitalWrite(BLUE, LOW);
        servoL.write(100);    //90 stoped 0 is forward     //100 //60
        servoR.write(103);    //93 stoped                 //103  //153
        delay(75);
      }
      if(switchState == 0 || switch2State == 0){
        state = 3;
      }
  }
    if(state == 2){
      Serial.println("your in state 2");
      if(proximityValue > 3200){//turn right
        servoL.write(80);
        servoR.write(83); 
      }
      else if(proximityValue < 2500){
        servoL.write(100);
        servoR.write(103);
      }
      else if ((proximityValue > 2500)&&(proximityValue < 3200)) {
      servoL.write(60);
       servoR.write(138);
      } 
      if(switchState == 0 || switch2State == 0){
        state = 4;
      }
      counter++;
    }
    if(state == 3){
         digitalWrite(BLUE, HIGH);
        digitalWrite(RED, LOW);
        //servoL.write(80);    //90 stoped 0 is forward  
         servoL.write(100);   
        servoR.write(83);    //95 stoped 
        delay(350);
        servoL.write(80);    //90 stoped 0 is forward  
         //servoL.write(100);   
        servoR.write(83);    //95 stoped 
        delay(800);
         digitalWrite(RED, HIGH);
        digitalWrite(BLUE, HIGH);
        servoL.write(60);    //90 stoped 0 is forward     
        servoR.write(138);    //95 stoped 
        delay(800);
        state = 2;
    }
    if(state == 4){
        servoL.write(90);    //90 stoped 0 is forward  
         //servoL.write(100);   
        servoR.write(83);    //95 stoped 
        delay(500);
         digitalWrite(RED, HIGH);
        digitalWrite(BLUE, HIGH);
        digitalWrite(RED, HIGH);
        digitalWrite(BLUE, HIGH);
        servoL.write(60);    //90 stoped 0 is forward     
        servoR.write(138);    //95 stoped 
        delay(500);
        state = 2;
    }
    if(state==5){
     servoL.write(60);
     Rsen = analogRead(A1);
     
     while(Rsen<RIGHT)
     {
       delay(1);
       Rsen = analogRead(A1);
     }
     
     Rsen = analogRead(A1);
     
     while(Rsen>=RIGHT)
     {
      delay(1);
      Rsen = analogRead(A1);      
     }
           Lsen = analogRead(A2);

     while(Lsen<LEFT)
     {
       delay(1);
       Lsen = analogRead(A2);     
     }
     delay(20);
     state = 1;
    }
      
    
}

// readProximity() returns a 16-bit value from the VCNL4000's proximity data registers
unsigned int readProximity()
{
  unsigned int data;
  byte temp;
  
  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x08);  // command the sensor to perform a proximity measure
  
  while(!(readByte(COMMAND_0)&0x20));  // Wait for the proximity data ready bit to be set
  data = readByte(PROXIMITY_RESULT_MSB) << 8;
  data |= readByte(PROXIMITY_RESULT_LSB);
  
  return data;
}

// readAmbient() returns a 16-bit value from the VCNL4000's ambient light data registers
unsigned int readAmbient()
{
  unsigned int data;
  byte temp;
  
  temp = readByte(COMMAND_0);
  writeByte(COMMAND_0, temp | 0x10);  // command the sensor to perform ambient measure
  
  while(!(readByte(COMMAND_0)&0x40));  // wait for the proximity data ready bit to be set
  data = readByte(AMBIENT_RESULT_MSB) << 8;
  data |= readByte(AMBIENT_RESULT_LSB);
  
  return data;
}

// writeByte(address, data) writes a single byte of data to address
void writeByte(byte address, byte data)
{
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

// readByte(address) reads a single byte of data from address
byte readByte(byte address)
{
  byte data;
  
  Wire.beginTransmission(VCNL4000_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(VCNL4000_ADDRESS, 1);
  while(!Wire.available());
  data = Wire.read();

  return data;
}
