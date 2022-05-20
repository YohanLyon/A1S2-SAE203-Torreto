#include "definefile.h"
#include "Ultrasonic.h"
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include "Adafruit_PWMServoDriver.h"
#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

//Bandeau Led
Adafruit_NeoPixel pixels(NUMPIXELS, A0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, D2, NEO_GRB + NEO_KHZ800);

//DCMotor
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *RoueGauche = AFMS.getMotor(1);
Adafruit_DCMotor *RoueDroite = AFMS.getMotor(3);

int speedBase;
int speedBase2;

String avancer = "avancer";
String reculer = "reculer";
String droite = "droite";
String gauche = "gauche";
String stoped = "stop";

boolean avancer2 = false;
boolean reculer2 = false;

//Bluethooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

//Capteur Ultrasons
Ultrasonic ultrasonic(D4);


void checking(String& message) {
    
    if (message == avancer) {

      avancer2 = true;
      reculer2 = false;
      
      for (int i = 0; i < NUMPIXELS; i++)
        pixels.setPixelColor(i, pixels.Color(0,10,0));  
      pixels.show();
      
      RoueDroite->setSpeed(speedBase); 
      RoueGauche->setSpeed(speedBase2); 
      RoueDroite->run(BACKWARD);
      RoueGauche->run(FORWARD);  
      message = " ";
        
    } else if (message == reculer) {
        
      avancer2 = false;
      reculer2 = true;
      
      for (int i = 0; i < NUMPIXELS; i++)
        pixels.setPixelColor(i, pixels.Color(10,0,0));  
      pixels.show();
      
      RoueDroite->setSpeed(speedBase); 
      RoueGauche->setSpeed(speedBase2);
      RoueDroite->run(FORWARD);
      RoueGauche->run(BACKWARD);  
        
      message = " ";
        
    } else if (message == droite) {
        
      pixels.setPixelColor(9, pixels.Color(15,2,0));
      pixels.setPixelColor(8, pixels.Color(15,2,0));

      if (avancer2 == true) {
          
        pixels.setPixelColor(1, pixels.Color(0,10,0));
        pixels.setPixelColor(0, pixels.Color(0,10,0)); 
          
      } else if (reculer2 == true) {
          
        pixels.setPixelColor(1, pixels.Color(10,0,0));
        pixels.setPixelColor(0, pixels.Color(10,0,0));  
      }
      
      pixels.show();
      
      RoueDroite->setSpeed(40); 
      RoueGauche->setSpeed(120);  
        
      message = " ";
        
    } else if (message == gauche) {
        
      pixels.setPixelColor(1, pixels.Color(15,2,0));
      pixels.setPixelColor(0, pixels.Color(15,2,0));

      if (avancer2 == true) {
          
        pixels.setPixelColor(8, pixels.Color(0,10,0));
        pixels.setPixelColor(9, pixels.Color(0,10,0)); 
          
      } else if (reculer2 == true) {
          
        pixels.setPixelColor(8, pixels.Color(10,0,0));
        pixels.setPixelColor(9, pixels.Color(10,0,0));  
      }
            
      pixels.show();
      
      RoueDroite->setSpeed(120); 
      RoueGauche->setSpeed(40);  
        
      message = " ";
        
    } else if (message == stoped) {
        
      avancer2 = false;
      reculer2 = false;
        
      pixels.clear();
      pixels.show();
        
      RoueDroite->setSpeed(speedBase); 
      RoueGauche->setSpeed(speedBase2);
      RoueDroite->run(RELEASE);
      RoueGauche->run(RELEASE); 
        
      message = " ";
    }
  }



void setup() {
  AFMS.begin();
  pinMode(A2, INPUT);
  pinMode(D4, INPUT);
  pinMode(A0, OUTPUT);
    
  Serial.begin(115200);
  SerialBT.begin("Toretto"); //Bluetooth device name
  Serial.println("The device started on  Bluetooth !");
  pixels.begin();
}




String message = " ";

void loop() {

  speedBase2 = ((analogRead(A2)*255)/4095);
  speedBase = ((analogRead(A2)*255)/4095)-3;
  
  if (SerialBT.available()) {
      
    char temp;
    temp = SerialBT.read();
      
    if (message == " ") {
        
      message = temp;
        
    } else {
        
      message += temp; 
    }
      
    message.trim();
  }
    
  pixels.show();
  checking(message);
  
  long RangeInCentimeters;
  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  
  if (RangeInCentimeters > 10 && avancer2 == true && RangeInCentimeters != 533) {

      avancer2 = false;
      reculer2 = false;
      RoueDroite->run(FORWARD);
      RoueGauche->run(BACKWARD);  
      
      for (int i = 0; i < NUMPIXELS; i++)
        pixels2.setPixelColor(i, pixels.Color(10,0,0));  
      pixels2.show();
      
      delay(1000);
      RoueDroite->run(RELEASE);
      RoueGauche->run(RELEASE);     
      pixels.clear();
      pixels.show();
      
    } else {
      pixels2.clear();
      pixels2.show();
    }
}
