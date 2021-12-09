#include <SoftwareSerial.h>
#include <Wire.h>  // Wire library - used for I2C communication
SoftwareSerial mySerial(2, 3); // TX, RX initialized
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // The values obtained from the accelerometer
bool label = true;
String dataIncoming1 = "Myoware";
String dataIncoming2 = "Accelerometer";
int input = 0;
int value;
float percent = 0.05;
int threshold = 1024*percent;

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(A0,INPUT);

  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);

  // Off-set Calibration
  //X-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1E);  // X-axis offset register
  Wire.write(1);
  Wire.endTransmission();
  delay(10);
  //Y-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x1F); // Y-axis offset register
  Wire.write(-2);
  Wire.endTransmission();
  delay(10);
  
  //Z-axis
  Wire.beginTransmission(ADXL345);
  Wire.write(0x20); // Z-axis offset register
  Wire.write(-7);
  Wire.endTransmission();
  delay(10);
}

void loop() {
  while(label){
    Serial.print(dataIncoming1);
    Serial.print(",");
    Serial.println(dataIncoming2);
    label = false;
  }
if (mySerial.available())       
  Serial.write(mySerial.read()); 
if (Serial.available())     
   mySerial.write(Serial.read());
input= analogRead(A0);
//mySerial.println(input);
//mySerial.println("\n");
//Serial.println(input);
//Serial.println("\n");

// === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/32; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/32;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/32;

if(value >=input+threshold || value <= input-threshold){
  mySerial.println(input);
  mySerial.println("\n");
  Serial.print(input);
  Serial.print(",");
  Serial.print(X_out);
  Serial.print(",");
  Serial.print(Y_out);
  Serial.print(",");
  Serial.println(Z_out);
  value = input;
}
delay(1000); 
}
