//Libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Wire.h>

// define the pins used
#define CLK 13       // SPI Clock, shared with SD card
#define MISO 12      // Input data, from VS1053/SD card
#define MOSI 11      // Output data, to VS1053/SD card

//Pins
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
// DREQ should be an Int pin
#define CARDCS 4     // Card chip select pin
#define DREQ 3

// 0x4B is the default i2c address of MAX9744 amp
#define MAX9744_I2CADDR 0x4B

//Create shield object
Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//vol pot
	int volPotPin = 1;
	int8_t volVal = 0;

	//Headphone and relay control pins
  int jackPin1 = 5;
  int jackPin2 = 2; //Need to change this since it references GPIO pins
  int speakerPin = 2;

  File root;

void setup()
{
	pinMode(jackPin1, INPUT);
	pinMode(speakerPin, OUTPUT);

	Wire.begin();

	Serial.begin(9600);
	Serial.println("Begin setup");

	//Initialize musicPlayer board
	Serial.println("Initializing VS1053 shield...");
	if(! musicPlayer.begin()) {
	    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
	    while(1);
	}
	Serial.println(F("VS1053 found"));

	//Initialize MicroSD card
	Serial.println("Initializing Micro SD card...");
	if(!SD.begin(CARDCS)) {
	    Serial.println(F("Initialization failed! Check SD card."));
	    while(1); //end
	} 
	Serial.println("Initialization complete");
	//End initialization

	  volVal = map(analogRead(volPotPin), 0, 1023, 0, 63);
  // Serial.println("Start volume:");
  // Serial.println(volVal);
  setVolume(volVal);

   musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  //Serial.println("Interupt pin ");

   root = SD.open("/test/");

   musicPlayer.GPIO_pinMode(jackPin2, INPUT);
   musicPlayer.setVolume(50,50);
   Serial.println("End setup");
}

void loop()
{
	delay(500);

	  int jack1 = digitalRead(jackPin1);
	  int jack2 = musicPlayer.GPIO_digitalRead(jackPin2);

	  //When headphones are plugged in it opens switch and sets jack pins to LOW
	  //If jack1 or jack2 is low (plugged in) make speakerpin HIGH (this flips relay from closed to open) 
	    if(jack1 == LOW || jack2 == LOW) {
	        digitalWrite(speakerPin, HIGH);
	        Serial.println("Headphone jack connected, speaker off");
	    }
	    //Else, if jacks aren't LOW, speakerPin should be LOW (relay is closed)
	    else {
	      digitalWrite(speakerPin, LOW);
	      Serial.println("Headphone jacks unplugged, speaker on");
	    }


	    volVal = map(analogRead(volPotPin), 0, 1023, 0, 63);
	    setVolume(volVal);

	    musicPlayer.startPlayingFile("1.mp3");
}

boolean setVolume(int8_t v) {
  //Serial.println("attempt volume set " + v);

  // cant be higher than 63 or lower than 0
  if (v > 63) v = 63;
  if (v < 0) v = 0;

  Serial.println("Volume:");
  Serial.println(v);
  Wire.beginTransmission(MAX9744_I2CADDR);
  Wire.write(v);
  if (Wire.endTransmission() == 0) {
    return true;
  } else {
    return false;
  }
}
