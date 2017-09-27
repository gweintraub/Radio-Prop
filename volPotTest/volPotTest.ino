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
 if(volVal != 0) {
    int computedVol = map(volVal, 0, 1023, 1, 10);
    Serial.println(computedVol);
 } else Serial.println("0");
	delay(100);
}
