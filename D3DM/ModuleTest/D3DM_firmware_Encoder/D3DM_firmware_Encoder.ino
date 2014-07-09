#define servo Serial1
#define wifi Serial2
#define laser Serial3

#define armA 40
#define armB 41

#define baseA 42
#define baseB 43

#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

float getbase_angle();
float getArmAngle();

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

boolean blinkState = false;

//base counter:
volatile unsigned int base_cnt = 0;

unsigned int base_A_old = 0;
unsigned int base_B_new = 0;
int base_step = 1;
static boolean base_rotating=false;      // debounce management
// interrupt service routine vars
boolean base_A_set = false;              
boolean base_B_set = false;

//arm counter:
volatile unsigned int arm_cnt = 0;
int arm_step = 1;
unsigned int arm_A_old = 0;
unsigned int arm_B_new = 0;

static boolean arm_rotating=false;      // debounce management
// interrupt service routine vars
boolean arm_A_set = false;              
boolean arm_B_set = false;

void setup() {

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
	
	/* servo motor serial 1 */
	servo.begin(9600);
	/* uart-wifi serial 2 */
	wifi.begin(115200);
	/* laser sensor serial 3 */
	laser.begin(115200);
	
	pinMode(baseA,INPUT);
	pinMode(baseB,INPUT);
	
	pinMode(armA,INPUT);
	pinMode(armB,INPUT);
	
	digitalWrite(armA, HIGH);
	digitalWrite(armB, HIGH);
	
	digitalWrite(baseA, HIGH);
	digitalWrite(baseB, HIGH);

	attachInterrupt(baseA, baseAInte, CHANGE);
	//attachInterrupt(baseB, baseBInte, CHANGE);
	
	attachInterrupt(armA, armAInte, CHANGE);
	//attachInterrupt(armB, armBInte, CHANGE);
	
    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() 
{
	arm_rotating = true;
	base_rotating = true;
	
	float base_angle = 0;
	float arm_angle = 0;
	int laser_data = 0;
	
	base_angle = getbase_angle();
	arm_angle = getarm_angle();
	laser_data = laser.parseInt();
	
	//print thru wifi:
	wifi.print(base_angle);
	wifi.print("\t");
	wifi.print(arm_angle);
	wifi.print("\t");
	wifi.println(laser_data);
	
	//print thru serial
	Serial.print(base_angle);
	Serial.print("\t");
	Serial.print(arm_angle);
	Serial.print("\t");
	Serial.println(laser_data);
	
    // blink LED to indicate activity
	blinkState = !blinkState;
	digitalWrite(LED_PIN, blinkState);
}

void ndelay(unsigned int n)
{
	while(n--);
}

float getbase_angle()
{
	return base_cnt*360/200;
}

float getarm_angle()
{
	return arm_cnt*360/100;
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent2()
{
	char c;
	//Serial.println("Getting command from serve...");
	while (wifi.available())
	{
		c = wifi.read();
		servo.write(c);
		Serial.write(c);
	}
	//Serial.println("Got command from serve!!!!!!!");
}

void baseAInte()
{
	if(digitalRead(baseA) == digitalRead(baseB))
		base_cnt--;
	else
		base_cnt++;
}
/*
void baseBInte()
{
	if(digitalRead(baseA))
		base_step = -1;
	else
		base_step = 1;
}
*/
void armAInte()
{
	if(digitalRead(armA) == digitalRead(armB))
		arm_cnt--;
	else
		arm_cnt++;
}
/*
void armBInte()
{
	if(digitalRead(armA))
		arm_step = -1;
	else
		arm_step = 1;
}
*/
/*soft debounce method : failed to minus, can only add;
void baseAInte()
{
	// debounce
	if ( base_rotating )
		ndelay (1);  // wait a little until the bouncing is done

	// Test transition, did things really change? 
	if( digitalRead(baseA) != base_A_set ) 
	{
		// debounce once more
		base_A_set = !base_A_set;

		// adjust counter + if A leads B
		if ( base_A_set && !base_B_set ) 
		  base_cnt += 1;

		base_rotating = false;  // no more debouncing until loop() hits again
	}
}

void baseBInte()
{
	if ( base_rotating )
		ndelay (1);
	
	if( digitalRead(baseB) != base_B_set )
	{
		base_B_set = !base_B_set;
    
		//  adjust counter - 1 if B leads A
		if( base_B_set && !base_A_set ) 
		  base_cnt -= 1;

		base_rotating = false;
	}
}

void armAInte()
{
	// debounce
	if ( arm_rotating == true );
	//	ndelay (1);  // wait a little until the bouncing is done
	
	// Test transition, did things really change? 
	if( digitalRead(armA) != arm_A_set ) 
	{
		// debounce once more
		arm_A_set = !arm_A_set;

		// adjust counter + if A leads B
		if ( arm_A_set && !arm_B_set ) 
		  arm_cnt += 1;

		arm_rotating = false;  // no more debouncing until loop() hits again
	}
}

void armBInte()
{
	if ( arm_rotating );
	//	ndelay (1);
	
	if( digitalRead(armB) != arm_B_set )
	{
		arm_B_set = !arm_B_set;
    
		//  adjust counter - 1 if B leads A
		if( arm_B_set && !arm_A_set ) 
		  arm_cnt -= 1;

		arm_rotating = false;
	}
}
*/