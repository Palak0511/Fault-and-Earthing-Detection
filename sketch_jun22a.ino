
#define BLYNK_TEMPLATE_ID "TMPL3GEGzUjuS"
#define BLYNK_TEMPLATE_NAME "energy meter"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

#include <Filters.h> //Library to use
 
//#define ZMPT101B A0 //Analog input
 
float testFrequency = 50; // test signal frequency (Hz)
float windowLength = 100/testFrequency; // how long to average the signal, for statistist, changing this can have drastic effect
// Test as you need
 
float RawValue = 0;
float Volts_TRMS; // estimated actual voltage in Volts
 
float intercept = 0; // to be adjusted based on calibration testin
float slope = 2.79;

unsigned long printPeriod = 1000; //Measuring frequency, every 1s, can be changed
unsigned long previousMillis = 0;
 
RunningStatistics inputStats;

char auth[] = "L3tcyADiOZ_UiPsW2OMhAYDbPQAuC1KN";
BlynkTimer timer;
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Laksh2";
char pass[] = "wplk1466";
// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D6,OUTPUT); 
  pinMode(D7,OUTPUT); 
  pinMode(D8,OUTPUT); 
  pinMode(A0, INPUT);
  emon1.current(A0, 111.1);
  inputStats.setWindowSecs(windowLength);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, ReadVoltage);

}

void loop() {
  // put your main code here, to run repeatedly:
//ReadCurrent();
ReadVoltage();
}

float ReadCurrent(){
digitalWrite(D6,0);
digitalWrite(D7,0);
digitalWrite(D8,0);
delay(10);
double Irms = emon1.calcIrms(1480);
delay(10);
  Serial.print(Irms*230.0);	       // Apparent power
  Serial.print("Irms =  ");
  Serial.println(Irms);		       // Irms
Blynk.virtualWrite(V0, Irms);
return Irms;
}

float ReadVoltage(){
float value;
  
digitalWrite(D6,1);
digitalWrite(D7,0);
digitalWrite(D8,0);
delay(10);
value = analogRead(A0);
delay(10);

RawValue = value; // read the analog in value:
inputStats.input(RawValue); // log to Stats function
 
if((unsigned long)(millis() - previousMillis) >= printPeriod) { //We calculate and display every 1s
previousMillis = millis(); // update time
 
Volts_TRMS = inputStats.sigma()* slope + intercept;
// Volts_TRMS = Volts_TRMS*0.979; //Further calibration if needed
 
/*Serial.print("Non Calibrated: ");
Serial.print("\t");
Serial.print(inputStats.sigma());
Serial.print("\t");
Serial.print("Calibrated: ");
Serial.print("\t");
*/
Serial.print("Voltrms = ");
Serial.println(Volts_TRMS);
Blynk.virtualWrite(V1, Volts_TRMS);
}
return Volts_TRMS;

}
