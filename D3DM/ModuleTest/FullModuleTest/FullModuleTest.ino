#define laser Serial1
#define wifi Serial2
#define wifiEvent() serialEvent2()

#define paramNum 2

#define BASE_MIN  1300
#define BASE_STOP 1480
#define BASE_MAX  1600
#define ARM_MIN   20
#define ARM_MAX   160

#include <AS5045.h>
#include <Servo.h>

//command format declaration:
char* sprt = "\t";
char* terminal = "\n";

//command buffer and isValid bit;
int param[paramNum];
boolean isValid = false;

//motion control variables
int basespd = 1480;
int armpos = 90;

//sensors instantiated
AS5045 enc1 (2, 3, 4) ;
AS5045 enc2 (48, 50, 52) ;

Servo base;
Servo arm;

//function prototype declaration:
boolean assert_param();
void clearParam();

void setup ()
{
  Serial.begin (115200) ;   // NOTE BAUD RATE
  
  /* uart-wifi serial 2 */
  wifi.begin(115200);
  /* laser sensor serial 3 */
  laser.begin(115200);
  
  // didn't program the OTP
  enc1.begin();
  enc2.begin();
  
  //base moves in full circle, arm moves within 180 degrees
  base.attach(6,800,1800);
  arm.attach(7,600,2190);
}

void loop ()
{

//serial0 debug output:
Serial.print(laser.parseInt());
Serial.print ("\t");
Serial.print (enc1.read () * 0.08789, DEC);
Serial.print ("\t");
Serial.println (enc2.read () * 0.08789 - 149.3320697, DEC);

//
wifi.print(laser.parseInt());
wifi.print ("\t");
wifi.print (enc1.read () * 0.08789, DEC);
wifi.print ("\t");
wifi.println (enc2.read () * 0.08789 - 149.3320697, DEC);

//set the movement when valid
if(isValid)
{
	base.write(basespd);
	arm.write(armpos);
	isValid = false;
}

delay(10);
}

void serialEvent()
{
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'A' : armpos = Serial.parseInt(); break;
      case 'B' : basespd = Serial.parseInt(); break;
      default : ;
    };
  }
}

void wifiEvent()
{
	boolean cmdGet;
	cmdGet = true;
	
	if(wifi.available() > 0)
	{
		for ( int i = 0; i < paramNum; i++)
		{
			if( wifi.findUntil(sprt, terminal) )
			{
				param[i] = wifi.parseInt();
			}
			else
			{
				cmdGet = false;
			}
		}
	}
	else
	{
		cmdGet = false;
	}
	
	if(cmdGet = true)
	{
		if(assert_param())
			isValid = true;
	}
	else
	{
		clearParam();
	}
}

boolean assert_param()
{
	if(param[0] > BASE_MIN && param[0] < BASE_MAX)
		if(param[1] > ARM_MIN && param[1] < ARM_MAX)
			return true;
	return false;
}

void clearParam()
{
	isValid = false;
	for(int i = 0; i < paramNum; i++)
		param[i] = -1;
}