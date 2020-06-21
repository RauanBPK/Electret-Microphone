/* 
Author: Tiago Oliveira Weber - 2018   - tiago (dot) oliveira (dot) weber (at) gmail (dot) com 
*/

//#define DEBUG


//********************************
// PWM 
//********************************
#define pwmpin 3

//********************************
// Include Libraries
//********************************
#include <stdarg.h>

//********************************
// Time Variables
//********************************
unsigned long start;
unsigned long time_millis;

//********************************
// Serial Communication Variables
//********************************
boolean serial_received = false;
int inByte = 0;         // incoming serial byte
int received_value = 128;
int old_value = 0;

// Functions
void p(char *fmt, ... ){ //http://playground.arduino.cc/Main/Printf
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);  
//  TCCR2B =(TCCR2B & 0xF8) | 0x01;
  start=millis();  // start time
  analogWrite(pwmpin, received_value); // just to start without serial command
}

void loop() {
   time_millis=millis()-start; // current time    
  //**********************************  
  // checks and receive data from serial
  //**********************************  
  serial_received = false; // resets serial_received variable  
  Serial.flush();
  if (Serial.available() > 0) {
    int i=0;
    while (Serial.available() > 0)
      {
	inByte = Serial.read();
        if (inByte==255)
            {
            received_value = Serial.read();
            serial_received = true;
            }        	
      }
  }


  
  //**********************************  
  // Read the input on analog pin 0:
  //**********************************  
  int adc_value = analogRead(A0);  
  
  //**********************************
  // Serial output          // this part is based on oscilloscope.h, from Sofian Audry
  //**********************************
  // Can't be written only as Serial.write(adc_value) because the ADC has a 10-bit resolution and we are sending 8 bits at a time
  // Will be on code only if not in debug mode
  #ifndef DEBUG 
  Serial.write(0xff);             // send init byte 1
  Serial.write(0x00);             // send init byte 2
  Serial.write(0xff);             // send init byte 3
  Serial.write(1);                // send channel
  Serial.write(adc_value >> 8); 
  Serial.write(adc_value);  
  
  // send long time_millis
  Serial.write(time_millis >> (8*3));
  Serial.write(time_millis >> (8*2));
  Serial.write(time_millis >> (8*1));
  Serial.write(time_millis >> (8*0));
  #endif

  //**********************************
  // ASCII output
  //**********************************
  // Print the values to the serial terminal for debugging purposes in the serial monitor
  // Will be on code only in debug
  #ifdef DEBUG
  p("Debug Mode\n");  
  if (serial_received) {
       p("Received value: %d", received_value);
        }
  #endif

  //**********************************
  // PWM output
  //**********************************
  if (serial_received && (received_value != old_value)) {
       analogWrite(pwmpin, received_value);
       old_value = received_value;
  }
       
  Serial.flush();
  //delay(100);
}

