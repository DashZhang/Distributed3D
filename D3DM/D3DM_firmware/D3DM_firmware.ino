//#define SERIAL_DEBUG

#define laser Serial1
#define wifi Serial2
#define wifiEvent() serialEvent2()

#define paramNum 2
#define initParamNum 10

#define STATIC_THRESHOLD 20

#define BASE_MIN  1200
#define BASE_MAX  1800

#define ARM_MIN   0
#define ARM_MAX   180

#define  ARM_AUTO_UNIT 1

#include <AS5045.h>
#include <Servo.h>
#include <PID_v1.h>

//initial parameters:
int magOffset[2] = {0 ,0};
int basePWLow = 800;
int basePWHigh = 1800;
int armPWLow = 600;
int armPWHigh = 2190;
int BASE_STOP = 1475;


//quad parameters
int leftLimit = 1024;
int rightLimit = 3072;
//vertical parameters
int vertical_delay_time = 100;
int vertical_spd = BASE_STOP + 150;
//half parameters
int half_spd = BASE_STOP + 150;
int half_angle_step = 1;

//command format declaration:
char* sprt = "\t";
char* terminal = "\r\n";
unsigned int mode = 0;

//command buffer and isValid bit;
int param[paramNum];
boolean isValid = false;
boolean baseValid = false;
boolean armValid = false;
boolean paramGet = false;

//initialization parameters
//magOffset[0]/[1], basePWLow/High, armPWLow/High
//lase two : BASE_STOP, MODE
int initParam[initParamNum];
int initParamMin[initParamNum] = {-4096, -4096, 500, 500, 500, 500, 1400, 0,   0,    0 };
int initParamMax[initParamNum] = {4096, 4096, 2500, 2500, 2500, 2500, 1600, 16,4096, 4096 };

//motion control variables

int basespd = 1480;

int armPos = 180;
int armPosStep = 1;
int arm_step = 1;

int arm_cnt_left = 0;
int arm_cnt_right = 0;

int arm_cnt_full = 0;

boolean armJumped = false;
boolean full_stop = false;

//data buffer

int laser_buffer = 0;
int enc1_buffer = 0;
int enc2_buffer = 0;


//sensors instantiated
AS5045 enc1 (2, 3, 4) ;
AS5045 enc2 (48, 50, 52) ;

Servo base;
Servo arm;

//function prototype declaration:

boolean assert_param();
void clear_param();
void print_init_param(UARTClass s);
void print_init_param(USARTClass s);
void print_param(UARTClass s);
void print_param(USARTClass s);
void print_data(UARTClass s);
void print_data(USARTClass s);
boolean spdSet(unsigned int spd);

boolean moveTo(unsigned int pos);

void setup ()
{

  /* initialize serial before everything else*/
  /* debug serial 0 */
  Serial.begin (115200) ;   // NOTE BAUD RATE
  
  /* uart-wifi serial 2 */
  wifi.begin(115200);
  wifi.setTimeout(10);
  /* laser sensor serial 3 */
  laser.begin(115200);
  
  wifi.println("hello everyone!");
  
  if( get_init_param() )
  {

#ifdef SERIAL_DEBUG
	Serial.println("using wifi param to init");
	print_init_param(Serial);
#endif

	wifi.println("using wifi param to init");
	print_init_param(wifi);
	//translate the initParam array
	magOffset[0] = initParam[0];
	magOffset[1] = initParam[1];
	
	basePWLow = initParam[2];
	basePWHigh = initParam[3];
	armPWLow = initParam[4];
	armPWHigh = initParam[5];
	
	BASE_STOP = initParam[6];
	mode = initParam[7];
	   
	// didn't program the OTP
	enc1.begin(magOffset[0]);
	enc2.begin(magOffset[1]);

	//base moves in full circle, arm moves within 180 degrees
	base.attach(6,basePWLow, basePWHigh);
	arm.attach(7,armPWLow, armPWHigh);
	
	if(mode == 1) //quad
	{
		leftLimit = initParam[8];
		rightLimit = initParam[9];
		
		wifi.println();
		wifi.println();
		wifi.println();
		wifi.println("mode = 1");
		wifi.println();
		wifi.println();
		wifi.println();
		basespd = BASE_STOP+50;
		base.write(basespd);
		arm.write(180);
	}
	else if(mode == 2) //vertical
	{
		vertical_delay_time = initParam[8];
		vertical_spd = initParam[9];
	}
	else if (mode == 3) //half
	{
		half_spd = param[8];
		half_angle_step = param[9];
		basespd = BASE_STOP + 150;
	}

  }
  //didn't get the init parameters
  else
  {
	wifi.println("initialize with default parameters");
	// didn't program the OTP
	enc1.begin();
	enc2.begin();

	//base moves in full circle, arm moves within 180 degrees
	base.attach(6,basePWLow, basePWHigh);
	arm.attach(7,armPWLow, armPWHigh);
  }
}

void loop ()
{

#ifdef SERIAL_DEBUG
	print_data(Serial);
#endif

	print_data(wifi);
	
	//mode : 0-manual, 1-quad, 2-vertical, 3-half
	if(mode == 0)
	{
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
				basespd = param[0];
			}
			if(armValid)
			{
				armPos = param[1];
			}
		}
	}
	else if (mode == 1)//quad
	{
		int base_pos;
		
		base_pos = enc1.read();
	
		if(base_pos < leftLimit)
		{
			arm_cnt_right = 0;
			basespd = BASE_STOP-50;
			
			if(arm_cnt_left == 0)
			{
				if(armPos >= 180)
					armPosStep = -ARM_AUTO_UNIT;
				else if (armPos < 85)
				{
					full_stop = true;
					armPosStep = ARM_AUTO_UNIT;
				}
				armPos += armPosStep;
			}
			arm_cnt_left++;
		}
		else if(base_pos > rightLimit)
		{
			arm_cnt_left = 0;
			basespd = BASE_STOP+50;
			
			if(arm_cnt_right == 0)
			{
				if(armPos >= 180)
					armPosStep = -ARM_AUTO_UNIT;
				else if (armPos < 85)
				{
					full_stop = true;
					armPosStep = ARM_AUTO_UNIT;
				}
				armPos += armPosStep;
			}
			arm_cnt_right++;
		}
		
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
			print_param(Serial);
#endif
			
			if(baseValid && armValid)
			{
				basespd = param[0];
				armPos = param[1];
				full_stop = true;
			}
		}
		
	}
	else if (mode == 2)//vertical
	{
		if(armPos >= 180)
		{
			armPosStep = -ARM_AUTO_UNIT;
			base.write(1500);
			delay(100);
			base.write(BASE_STOP);
		}
		else if (armPos < 10)
		{
			armPosStep = ARM_AUTO_UNIT;
			base.write(1500);
			delay(130);
			base.write(BASE_STOP);
		}
		armPos += armPosStep;

		if(paramGet)
		{
			paramGet = false;
			isValid = assert_param();
		}

		if(isValid)
		{
			isValid = false;

#ifdef SERIAL_DEBUG
			Serial.println("valid command get---------");
			print_param(Serial);
#endif
			
			if(baseValid && armValid)
			{
				basespd = param[0];
				armPos = param[1];
				full_stop = true;
			}
		}
		
	}
	else if (mode == 3)//half
	{
		if(enc1.read() > 4000)
		{
			arm_cnt_full++;
			
			if(arm_cnt_full == 1)
			{
				if(armPos >= 180)
					armPosStep = -ARM_AUTO_UNIT;
				else if (armPos < 30)
				{
					full_stop = true;
					armPosStep = ARM_AUTO_UNIT;
				}
				armPos += armPosStep;
			}			
		}
		else
		{
			arm_cnt_full = 0;
		}
		
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
			print_param(Serial);
#endif
			
			if(baseValid && armValid)
			{
				basespd = param[0];
				armPos = param[1];
				full_stop = true;
			}
		}
		
	}
	else
	{
		basespd = BASE_STOP;
		armPos = 90;
	}
	
	if(full_stop)
	{
		base.write(BASE_STOP);
		arm.write(90);
	}
	else
	{
		//change spd/pos every loop
		base.write(basespd);
		arm.write(armPos);
	}
	
}

boolean get_init_param()
{
	int timesTried = 0;
	
	while(timesTried < 3000)
	{
		wifi.print("initialize try : ");
		wifi.print(timesTried+1);
		wifi.print(" ");
		//while( wifi.available() <= 0 );
		
		for(int i = 0; i < initParamNum; i++)
		{
			initParam[i] = wifi.parseInt();
		}
		
		if( assert_init_param() )
		{
			break;
		}
		timesTried ++;
		wifi.print("try again after 1s: ");
		wifi.println();
		delay(1000);
	}
}

#ifdef SERIAL_DEBUG
//serial debug
void serialEvent()
{
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case 'A' : armPos = Serial.parseInt(); break;
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

boolean stopRotation()
{
	int delta_step = 0;
	boolean steady = true;
	
	do
	{
		delta_step = enc1.read_bias();
		wifi.print("read_bias : ");
		wifi.println(delta_step);
		delay(50);
		delta_step -= enc1.read_bias();
		wifi.print("read_bias - read_bias: ");
		wifi.println(delta_step);

		if(delta_step > 400)
		{
			delta_step = 400;
		}
		
		if(delta_step < -400)
		{
			delta_step = -400;
		}

		base.write( base.readMicroseconds() + delta_step/10 );
		wifi.print("delta_step : ");
		wifi.println(delta_step);

	}while(delta_step < -STATIC_THRESHOLD || delta_step > STATIC_THRESHOLD);
	
	return steady;
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

boolean assert_init_param()
{
	boolean initParamValid[initParamNum];
	boolean initParamValidAsserted = true;
	for(int i = 0; i < initParamNum; i++)
	{
		initParamValid[i] = ( initParam[i] >= initParamMin[i] && initParam[i] <= initParamMax[i]);
		initParamValidAsserted = initParamValidAsserted && initParamValid[i];
	}
	
#ifdef SERIAL_DEBUG
	if( !baseValid && !armValid)
	{
		Serial.println();
		Serial.print("assert failed init param : ");
		print_init_param(Serial);
	}
#endif

	return (initParamValidAsserted);
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
	laser_buffer = laser.parseInt();
	enc1_buffer = enc1.read_bias();
	enc2_buffer = enc2.read_bias();
	
	s.print (enc1_buffer, DEC);
	s.print ("\t");
	s.print (enc2_buffer, DEC);
	s.print ("\t");
	s.println(laser_buffer, DEC);
}

void print_data(USARTClass s)
{
	laser_buffer = laser.parseInt();
	enc1_buffer = enc1.read_bias();
	enc2_buffer = enc2.read_bias();
	
	s.print (enc1_buffer, DEC);
	s.print ("\t");
	s.print (enc2_buffer, DEC);
	s.print ("\t");
	s.println(laser_buffer, DEC);
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

void print_init_param(UARTClass s)
{
	for(int i = 0; i < initParamNum; i++)
	{
		s.print("init param ");
		s.print(i+1);
		s.print(" : ");
		s.print(initParam[i]);
		s.print("\t");
	}
	s.println();
}

void print_init_param(USARTClass s)
{
	for(int i = 0; i < initParamNum; i++)
	{
		s.print("init param ");
		s.print(i+1);
		s.print(" : ");
		s.print(initParam[i]);
		s.print("\t");
	}
	s.println();
}
