int volPotPin = 1;
int volVal = 0;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	int volVal = analogRead(volPotPin);
	Serial.println(volVal);
//  int computedVol = map(volVal, 0, 1023, 0, 63);
//  Serial.println(computedVol);
	delay(100);
}
