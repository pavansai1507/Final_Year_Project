
/*This code is written by using the Embedded C with the help of ArduinoUNO software and also included the version*/ 
#include <TinyGPS++.h> 
#include <Wire.h> 
#include <LiquidCrystal.h> 
#include <Adafruit_Sensor.h> 
#include <Adafruit_ADXL345_U.h> 
#include <SoftwareSerial.h> 
#include <LiquidCrystal.h> 
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for 
most acurate BPM math. 
#include <PulseSensorPlayground.h>     
// Includes the PulseSensorPlayground 
Library.    
#include <OneWire.h> 
#include <DallasTemperature.h> 
#define ONE_WIRE_BUS 4 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire); 
SoftwareSerial uart(2, 3); 
const int rs = 12, en = 11, d4 = A2, d5 = A3, d6 = 7, d7 = 6; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); 
const int PulseWire = A0; 
const int LED = LED_BUILTIN; 
int Threshold = 550; 
TinyGPSPlus gps; 
String LAT, LON; 
PulseSensorPlayground pulseSensor;
const int mq3Pin = A1; 
const int trigPin = 8; 
const int echoPin = 9; 
const int buzzer = 13; 
const int relaypin = 5; 
const int blinkPin = 10; 
String msg; 
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345); 
void setup() { 
Serial.begin(9600); 
uart.begin(9600); 
pulseSensor.analogInput(PulseWire); 
pulseSensor.blinkOnPulse(LED); 
pulseSensor.setThreshold(Threshold); 
pinMode(trigPin, OUTPUT); 
pinMode(echoPin, INPUT); 
pinMode(buzzer, OUTPUT); 
pinMode(relaypin, OUTPUT); 
pinMode(blinkPin, INPUT); 
digitalWrite(relaypin, LOW); 
digitalWrite(buzzer, LOW); 
lcd.begin(16, 2); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Alcohol"); 
lcd.setCursor(0, 1); 
lcd.print("Detection system"); 
delay(1000); 
Serial.println("ADXL345 TEST"); 
while (!accel.begin()) { 
Serial.println("No valid sensor found"); 
delay(1000); 
} 
if (pulseSensor.begin()) { 
Serial.println("We created a pulseSensor Object !");  //This prints one time at 
Arduino power-up,  or on Arduino reset. 
} 
} 
void loop() { 
smartDelay(5000); 
sensors_event_t event; 
accel.getEvent(&event); 
sensors.requestTemperatures(); 
float tempC = sensors.getTempCByIndex(0); 
int sensorValue = analogRead(mq3Pin); 
Serial.print(sensorValue); 
delay(1000); 
int blinkState = digitalRead(blinkPin); 
Serial.print("   Sensor Value: "); 
Serial.print(sensorValue); 
delay(1000); 
long duration; 
int distance; 
// Triggering the ultrasonic sensor 
digitalWrite(trigPin, LOW); 
delayMicroseconds(2); 
digitalWrite(trigPin, HIGH); 
delayMicroseconds(10); 
digitalWrite(trigPin, LOW); 
// Receiving the echo and calculating the distance 
duration = pulseIn(echoPin, HIGH); 
distance = duration * 0.034 / 2;  // Speed of sound is 343 meters/second or 0.034 
centimeters/microsecond 
// Print the distance to the Serial Monitor 
Serial.print(" Vechicle Detected at a Distance: "); 
Serial.print(distance); 
Serial.println(" cm"); 
delay(1000); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Distance: "); 
lcd.print(distance); 
lcd.println(" cm"); 
if (distance < 20) { 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Vehicle infront"); 
lcd.setCursor(0, 1); 
lcd.print("Of You"); 
delay(1000); 
digitalWrite(buzzer, HIGH); 
delay(1000); 
} else if (pulseSensor.sawStartOfBeat()) { // Constantly test to see if "a beat happened". 
int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our 
pulseSensor object that returns BPM as an "int". 
// "myBPM" hold this BPM value now. 
Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message 
"a heartbeat happened". 
Serial.print("BPM: "); 
                       //
 Print phrase "BPM: " 
Serial.println(myBPM);                        // Print the value inside of myBPM
delay(1000); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("BPM: "); 
lcd.setCursor(0, 1); 
lcd.print(myBPM); 
delay(3000); 
} 
if (tempC != 0) { 
Serial.println("Temperature:"); 
Serial.println(tempC ); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Temperature"); 
lcd.setCursor(0, 1); 
lcd.print(tempC); 
delay(3000); 
} 
if (sensorValue > 400) { 
Serial.println("Alcohol Detected!"); 
delay(1000); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print(" Alcohol detected"); 
delay(1000); 
digitalWrite(buzzer, HIGH); 
delay(1000); 
digitalWrite(relaypin, HIGH); 
delay(1000); 
msg = "Alcohol detected"; 
SendMessage(); 
delay(1000); 
return; 
} if (blinkState == 0) { 
// Actions when eye blink is detected 
Serial.println("Drowsiness Detected!!"); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Drowsiness"); 
delay(1000); 
digitalWrite(buzzer, HIGH); 
delay(1000); 
digitalWrite(relaypin, HIGH); 
delay(1000); 
} if ((event.acceleration.y) > 7 || (event.acceleration.y) < -7 || 
(event.acceleration.x) > 7 || (event.acceleration.x) < -7) { 
Serial.print("Accident detected with ADXL345!"); 
delay(500); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Y:"); 
lcd.print(event.acceleration.y); 
lcd.setCursor(0, 1); 
lcd.print("Accident detected"); 
delay(1000); 
digitalWrite(buzzer, HIGH); 
delay(1000); 
digitalWrite(relaypin, HIGH); 
delay(1000); 
msg = "Accident detected";
SendMessage(); 
} else if ((event.acceleration.x) > 7 || (event.acceleration.x) < -7) { 
Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  "); 
Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  "); 
Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  "); 
Serial.println("m/s^2"); 
delay(1000); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("X:"); 
lcd.print(event.acceleration.x); 
lcd.setCursor(0, 1); 
lcd.print("Accident detected"); 
delay(1000); 
digitalWrite(relaypin, HIGH); 
delay(1000); 
msg = "Accident detected"; 
SendMessage(); 
return; 
}  
String LAT = String(gps.location.lat(), 6); 
String LON = String(gps.location.lng(), 6); 
Serial.print("LAT:"); 
Serial.println(LAT); 
Serial.print("LON:"); 
Serial.println(LON); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("lat: "); 
lcd.print(LAT);
lcd.setCursor(0, 1); 
lcd.print("lng: "); 
lcd.print(LON); 
delay(1000); 
} 
void SendMessage() 
{ 
uart.println("AT+CMGF=1\r"); 
delay(2000); 
uart.println("AT+CMGS=\"+919346040428\"\r"); 
// Replace x with mobile number 
delay(2000); 
uart.print(msg); 
uart.println("http://www.google.com/maps/place/"  + String(gps.location.lat(), 6) 
+ " ," + String(gps.location.lng(), 6)); 
uart.write(26); 
lcd.clear(); 
lcd.setCursor(0, 0); 
lcd.print("Message sent"); 
delay(2000); 
} 
static void smartDelay(unsigned long ms) 
{ 
unsigned long start = millis(); 
do 
{ 
while (uart.available())  /* Encode data read from GPS while data is available on 
serial port */ 
gps.encode(uart.read()); 
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */ 
} while (millis() - start < ms); 
}
