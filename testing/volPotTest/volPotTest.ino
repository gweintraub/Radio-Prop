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
 
 Serial.println(map(volVal, 0, 1023, 255, 0));
 
	delay(500);
}
