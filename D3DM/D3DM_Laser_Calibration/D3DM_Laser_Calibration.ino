void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
Serial3.begin(115200);
}

int cnt;

void loop()
{
  cnt = 0;
  for(int i = 0; i < 50; i++)
  {
    cnt += Serial3.parseInt();
  }
  Serial.println(cnt/50);
}
