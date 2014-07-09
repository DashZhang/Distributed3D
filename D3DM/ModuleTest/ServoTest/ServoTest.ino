/* 
 Controlling a servo position using a potentiometer (variable resistor) 
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int pos = 0;    // variable to read the value from the analog pin

void setup()
{
  Serial.begin(9600);
  myservo.attach(7,600,2190);  // attaches the servo on pin 9 to the servo object
}

void loop() 
{ 
  myservo.write(pos);                  // sets the servo position according to the scaled value 
  delay(15);                           // waits for the servo to get there 
} 

void serialEvent()
{
  if(Serial.available())
  {
    pos = Serial.parseInt();
    Serial.println(pos);
  }
}
