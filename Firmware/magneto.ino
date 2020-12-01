

#include <Wire.h>      //I2C Arduino Library - inbuilt into Arduino IDE (no library download required)
#define address 0x1E   //I2C address of the Magnetometer

//Global variables
int x,y,z;                   //Variables used to hold the triple axis data
int xCal=0, yCal=0, zCal=0;  //Variables used to calibrate the triple axis data.
int calValue = 1000;         //On startup, all axis variables will equal this number.



void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();

  //Setup the magnetometer 
  magSetting(0x00, B01101000); // Magnetometer settings associated with Register A. See datasheet for acceptable values. 
  magSetting(0x01, B01100000); // Magnetometer settings associated with Register B. See datasheet for acceptable values. 
  calibrateMag();              // Set or calibrate all axis variables to the calValue.
}

void loop(){
  getReadings();
  printReadings();
}


void magSetting(byte regLoc, byte setting){
  Wire.beginTransmission(address);
  Wire.write(regLoc); 
  Wire.write(setting);
  Wire.endTransmission();
  delay(10);
}


void getReadings(){
  magSetting(0x02, 0x01);        //prepare to take reading (Single measurement mode) - this populates the registers with data
  Wire.requestFrom(address, 6);  //Request 6 bytes. Each axis uses 2 bytes.
  if (Wire.available()>5){
     x = readValue()- xCal;
     z = readValue()- zCal;
     y = readValue()- yCal;    
  } else {
    Serial.println("****Error: Less than 6 bytes available for reading*****");
  }
}


int readValue(){
  int val = Wire.read()<<8; 
      val |= Wire.read();

  return val;
}


void printReadings(){
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
}


void calibrateMag(){
  getReadings();
  xCal = x-calValue;
  yCal = y-calValue;
  zCal = z-calValue;
}
