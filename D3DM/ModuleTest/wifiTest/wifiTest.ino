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
char* terminal = "\r\n";

//command buffer and isValid bit;
int param[paramNum];
boolean isValid = false;
boolean paramGet = false;

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
void clear_param();
void print_param(UARTClass s);
void print_param(USARTClass s);
void print_data(UARTClass s);
void print_data(USARTClass s);

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
  
  base.write(BASE_STOP);
  arm.write(90);
  
}

void loop ()
{
	//print_data(Serial);
	//rint_data(wifi);
	//begin command transmission:

	if(paramGet)
	{
		paramGet = false;
		isValid = assert_param();
	}

	//show the received command:
	if(isValid)
	{
		isValid = false;
		for(int i = 0; i < paramNum; i++)
		{
			print_param(Serial);
			print_param(wifi);
		}
		//base.write(basespd);
		//arm.write(armpos);
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
	
	Serial.println("------------------------------------------------");
	
	if(wifi.available() > 0 && wifi.peek() != 13)
	{
		for ( int i = 0; i < paramNum; i++)
		{
			if( wifi.findUntil(sprt, terminal) )
			{
				param[i] = wifi.parseInt();
			}
			else
			{
				Serial.println("didn't get enough param!");
				cmdGet = false;
			}
		}
	}
	else
	{
		Serial.println("no incoming buffer to read");
		cmdGet = false;
	}
	paramGet = cmdGet;
}

boolean assert_param()
{
	if(param[0] > BASE_MIN && param[0] < BASE_MAX)
	{
		if(param[1] > ARM_MIN && param[1] < ARM_MAX)
		{
			return true;
		}
	}
	
	Serial.print("assert failed data : ");
	for(int i = 0; i < paramNum; i++)
	{
		Serial.print("param ");
		Serial.print(i+1);
		Serial.print(" : ");
		Serial.print(param[i]);
		Serial.print("\t");
	}
	
	Serial.println("param range mismatch!");
	return false;
}

void clear_param()
{
	isValid = false;
	Serial.println("param cleared!");
	for(int i = 0; i < paramNum; i++)
		param[i] = -1;
}

void print_data(UARTClass s)
{
	s.print(laser.parseInt());
	s.print ("\t");
	s.print (enc1.read () * 360 / 4096, DEC);
	s.print ("\t");
	s.println (enc2.read () * 360 / 4096 - 149.3320697, DEC);
}

void print_data(USARTClass s)
{
	s.print(laser.parseInt());
	s.print ("\t");
	s.print (enc1.read () * 360 / 4096, DEC);
	s.print ("\t");
	s.println (enc2.read () * 360 / 4096 - 149.3320697, DEC);
}

void print_param(UARTClass s)
{
	for(int i = 0; i < paramNum; i++)
	{
		s.print("param ");
		s.print(i+1);
		s.print(" : ");
		s.print(param[i]);
		s.print("\t");
	}
	s.println();
}

void print_param(USARTClass s)
{
	for(int i = 0; i < paramNum; i++)
	{
		s.print("param ");
		s.print(i+1);
		s.print(" : ");
		s.print(param[i]);
		s.print("\t");
	}
	s.println();
}