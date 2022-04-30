#include <IRremote.h>
#include <IRremoteInt.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include "IRremote.h"

/*----------( declare objects )----------*/
LiquidCrystal myLCD(2,3,4,5,6,7);
Servo myServo;
IRrecv irrecv(11);

/*----------( declare pins )----------*/
const int servoPin = 9;     // digital pin for servo motor
const int lightPin = A1;    // analog pin for photocell
const int tempPin = A0;   // analog pin for thermistor
const int redLED = A7;    // analog pin for red LED 
const int grnLED = A5;    // analog pin for green LED
const int whtLED = A6;    // analog pin for white LED


/*----------( declare variables )----------*/
bool blindAction = false;   // used to track motor position
bool irOverride = false;    // used to allow remote override
int motorPosition = 0;    // initial setting for motor position
const int fullOpen = 90;    // degrees of servo for blinds full open
const int partialOpen = 45;   // degrees of servo for blinds partially closed
const int fullClose = 0;    // degrees of servo for blinds fully closed
int irCode = 00;    // code used to allow manual override

/********************************************************************/

decode_results results;

/*-----( Declare Functions )-----*/
void translateIR() // takes action based on IR code received

// describing Remote IR codes 

{
  switch(results.value)
  {
    //case 0xFFA25D: Serial.println("POWER");   break;
    //case 0xFFE21D: Serial.println("FUNC/STOP");   break;
    //case 0xFF629D: Serial.println("VOL+");    break;
    //case 0xFF22DD: Serial.println("FAST BACK");   break;
    //case 0xFF02FD: Serial.println("PAUSE");   break;
    //case 0xFFC23D: Serial.println("FAST FORWARD");    break;
    case 0xFFE01F: fullyOpen();    break;
    //case 0xFFA857: Serial.println("VOL-");    break;
    case 0xFF906F: fullyClose();    break;
    //case 0xFF9867: Serial.println("EQ");    break;
    //case 0xFFB04F: Serial.println("ST/REPT");    break;
    //case 0xFF6897: Serial.println("0");    break;
    //case 0xFF30CF: Serial.println("1");    break;
    //case 0xFF18E7: Serial.println("2");    break;
    //case 0xFF7A85: Serial.println("3");    break;
    //case 0xFF10EF: Serial.println("4");    break;
    //case 0xFF38C7: Serial.println("5");    break;
    //case 0xFF5AA5: Serial.println("6");    break;
    //case 0xFF42BD: Serial.println("7");    break;
    //case 0xFF4AB5: Serial.println("8");    break;
    //case 0xFF52AD: Serial.println("9");    break;
    //case 0xFFFFFFFF: Serial.println(" REPEAT");   break;  

  //default: 
    //Serial.println(" other button   ");

  } // end switch-case

  
  if(results.value == 0xFF30CF)
    {
      if(irCode == 3 || irCode == 33)
      {
        irCode = 0;
      }    
      if(irCode == 0 || irCode == 1)
      {
        irCode = (irCode * 10) + 1;
        Serial.println(irCode);
        analogWrite(whtLED, 255);
        delay(500);
        analogWrite(whtLED, 0);
      }
    }

  if(results.value == 0xFF7A85)
  {
    if(irCode == 1 || irCode == 11)
    {
      irCode = 0;
    }
    if(irCode == 0 || irCode == 3)
    {    
      irCode = (irCode * 10) + 3;
      Serial.println(irCode);
      analogWrite(whtLED, 255);
      delay(500);
      analogWrite(whtLED, 0);
    }
  }  


  delay(500); // Do not get immediate repeat
}


void blindPosition()
{
    myServo.write(motorPosition);
    delay(1000);
}

void fullyOpen()
{
    Serial.println("DOWN");   
    motorPosition = fullOpen;
    blindPosition();
}

void partiallyOpen()
{
    motorPosition = partialOpen;
    blindPosition();    
}

void fullyClose()
{
    Serial.println("UP");
    motorPosition = fullClose;
    blindPosition();    
}

/********************************************************************/
/********************************************************************/
void setup() {
  myServo.attach(9);
  myServo.write(fullClose);
  myLCD.begin(16,2);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode"); 
  irrecv.enableIRIn(); // Start the receiver

  pinMode(redLED, OUTPUT);
  pinMode(grnLED, OUTPUT);
  pinMode(whtLED, OUTPUT);
 
  analogWrite(grnLED, 255);
}
/********************************************************************/
/********************************************************************/

void loop() { 

  float lightReading = analogRead(lightPin);
  float tempReading = analogRead(tempPin);

  // This is OK
  double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
  tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK );       //  Temp Kelvin
  float tempC = tempK - 273.15;            // Convert Kelvin to Celcius
  float tempF = (tempC * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit

  // Display temperature in Fahrenheit
  myLCD.setCursor(0,0);
  myLCD.print("Temp         F  ");
  myLCD.setCursor(6, 0);
  myLCD.print(tempF);
  delay(500);  

  if(irCode == 11)
  {
    irOverride = true;
    analogWrite(grnLED, 0);
    analogWrite(redLED, 255);    
  }
  if(irCode == 33)
  {
    irOverride = false;
    analogWrite(redLED, 0);
    analogWrite(grnLED, 255);
  }

  if(!irOverride)
  {
    if(lightReading < LIGHT_TRIGGER)
    {
      fullyOpen();      
    }
    else if(tempF > TEMP_TRIGGER)
    {
      partiallyOpen();      
    }
    else if(lightReading >= LIGHT_TRIGGER)
    {
      fullyClose();      
    }
  }

  if(irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR(); 
    irrecv.resume(); // receive the next value
  } 
}