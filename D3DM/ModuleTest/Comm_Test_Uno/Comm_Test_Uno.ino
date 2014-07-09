#define wifi Serial
#define wifiEvent() serialEvent()

#define paramNum 2

#define BASE_MIN  1300
#define BASE_STOP 1480
#define BASE_MAX  1600

#define ARM_MIN   20
#define ARM_MAX   160

//command format declaration:
char* sprt = "\t";
char* terminal = "\r\n";

//command buffer and isValid bit;
int param[paramNum];
boolean isValid = false;
boolean paramGet = false;

int print_cnt = 0;

//function prototype declaration:
boolean assert_param();
void clear_param( );
void print_param( );
void print_data( );

void setup ()
{
  ///Serial.begin (115200) ;   // NOTE BAUD RATE
  
  /* uart-wifi serial 2 */
  wifi.begin(115200);
  /* laser sensor serial 3 */
  //laser.begin(115200);
  
  // didn't program the OTP
  //enc1.begin();
  //enc2.begin();
  
  //base moves in full circle, arm moves within 180 degrees  
}

void loop ()
{
	//print_data(Serial);
	//print_data(wifi);
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
		//print_param(Serial);
		print_param();
		//base.write(basespd);
		//arm.write(armpos);
	}

	//delay(10);
}

void wifiEvent()
{
	boolean cmdGet = false;
	cmdGet = true;
	
	Serial.println("------------------------------------------------");
	
	if(wifi.available() > 0)
	{
		for ( int i = 0; i < paramNum; i++)
		{
			if( wifi.findUntil(sprt, terminal) )
			{
                                Serial.println(Serial.peek());
				param[i] = wifi.parseInt();
			}
			else
			{
				Serial.println("didn't get enough param!");
				cmdGet = false;
			}
		}
                if( !wifi.find(terminal) )
                {
                  cmdGet = false;
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

void print_data( )
{
	wifi.print(2.333);
	wifi.print ("\t");
	wifi.print (4.555);
	wifi.print ("\t");
	wifi.println (6666);
}

void print_param( )
{
    wifi.print(print_cnt++);
    wifi.print("  :  ");
	for(int i = 0; i < paramNum; i++)
	{
		wifi.print("param ");
		wifi.print(i+1);
		wifi.print(" : ");
		wifi.print(param[i]);
		wifi.print("\t");
	}
	wifi.println();
}
