//#define SERIAL_DEBUG

#define laser Serial1
#define laserEvent() serialEvent1()
#define wifi Serial2
#define wifiEvent() serialEvent2()

#define paramNum 2

#define BASE_MIN  1200
#define BASE_STOP 1475
#define BASE_MAX  1800

#define ARM_MIN   0
#define ARM_MAX   180

#include <AS5045.h>
#include <Servo.h>

//command format declaration:
char* sprt = "\t";
char* terminal = "\r\n";

//command buffer and isValid bit;
int param[paramNum];
boolean isValid = false;
boolean baseValid = false;
boolean armValid = false;
boolean paramGet = false;

//motion control variables
int basespd = 1480;
int armpos = 90;

//sensors instantiated
AS5045 enc1 (2, 3, 4) ;
AS5045 enc2 (48, 50, 52) ;

//laser data
int distance = 0;


Servo base;
Servo arm;

//function prototype declaration:

boolean assert_param();
void clear_param();
void print_param(UARTClass s);
void print_param(USARTClass s);
void print_data(UARTClass s);
void print_data(USARTClass s);
boolean get_init_param();

void setup ()
{

  /* initialize serial before everything else*/
  /* debug serial 0 */
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

//int basespd = 1600;

int armPos = 90;

int posCnt = 0;

int step = 1;

void loop ()
{

#ifdef SERIAL_DEBUG
	print_data(Serial);
#endif

	print_data(wifi);
	
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

#ifdef SERIAL_DEBUG
		Serial.println("valid command get---------");
#endif

#ifdef SERIAL_DEBUG
		print_param(Serial);
#endif
		
		//print_param(wifi);
		
		if(baseValid)
		{
			base.write(param[0]);
		}
		if(armValid)
		{
			arm.write(param[1]);
		}
	}
}

boolean get_init_param()
{
	
}

#ifdef SERIAL_DEBUG
//serial debug
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
#endif

void wifiEvent()
{
	boolean cmdGet = false;
	cmdGet = true;

#ifdef SERIAL_DEBUG
	Serial.println("------------------------------------------------");
#endif
	
	if(wifi.available() > 0)
	{
		for ( int i = 0; i < paramNum; i++)
		{
			if( wifi.findUntil(sprt, terminal) )
			{

#ifdef SERIAL_DEBUG
				Serial.println(Serial.peek());
#endif

				param[i] = wifi.parseInt();
			}
			else
			{
			
#ifdef SERIAL_DEBUG
				Serial.println("didn't get enough param!");
#endif

				cmdGet = false;
			}
		}
		
		//use terminal '\r\n' to confirm transmission
		if( !wifi.find(terminal) )
		{
		  cmdGet = false;
		}
	}
	else
	{
	
#ifdef SERIAL_DEBUG
		Serial.println("no incoming buffer to read");
#endif

		cmdGet = false;
	}
	paramGet = cmdGet;
}

boolean assert_param()
{
	if(param[0] > BASE_MIN && param[0] < BASE_MAX)
	{
		baseValid = true;
	}
	else
	{
		baseValid = false;
	}
	
	if(param[1] > ARM_MIN && param[1] < ARM_MAX)
	{
		armValid = true;
	}
	else
	{
		armValid = false;
	}
	
#ifdef SERIAL_DEBUG
	if( !baseValid && !armValid)
	{
		Serial.println();
		Serial.print("assert failed data : ");
		print_param(Serial);
	}
#endif

	return (baseValid || armValid);
}

void clear_param()
{
	isValid = false;
	
#ifdef SERIAL_DEBUG
	Serial.println("param cleared!");
#endif

	for(int i = 0; i < paramNum; i++)
		param[i] = -1;
}

void print_data(UARTClass s)
{
	int laser_data = 0;
	s.print (enc1.read (), DEC);
	s.print ("\t");
	s.print (enc2.read (), DEC);
	s.print ("\t");
	for(int i = 0; i < 100; i++)
		laser_data += laser.parseInt();
	s.println( int(laser_data/100) );
}

void print_data(USARTClass s)
{
	s.print (enc1.read (), DEC);
	s.print ("\t");
	s.print (enc2.read (), DEC);
	s.print ("\t");
	s.println(laser.parseInt());
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
