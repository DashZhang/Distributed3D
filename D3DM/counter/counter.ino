void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

pinMode(2,INPUT);
pinMode(3,INPUT);

attachInterrupt(1, dir, RISING);

}
int n;
void loop() 
{
  // put your main code here, to run repeatedly:
Serial.println(n);
}

void dir()
{
  detachInterrupt(0);
  if(digitalRead(2))
    attachInterrupt(0, plus, RISING);
  else
    attachInterrupt(0, minus, RISING);
  
}

void plus()
{
  n++;
}

void minus()
{
  n--;
}