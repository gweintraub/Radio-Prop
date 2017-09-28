/*************************************************** 
	This Arduino class is made for a custom radio sound box
	Made by Gabe Weintraub for The Factory NYC, based on the original design by Rachel Ciavarella.

	gabe.weintraub@gmail.com
	rachel.ciavarella@gmail.com

	Relevant boards and libraries:
	Adafruit Micro SD breakout
	Adafruit MAX9744 breakout
	Adafruit VS1053 breakout board w/o amp



	Pinouts for Arduino UNO. Finally will probablby be built on Micro
	Digital:
	SHIELD_DCS = 7
	SHIELD_CS = 6
	CARDCS = 4
	DREQ = 3
	SHIELD_RESET = 9
	SpeakerPin/Relay Signal = 2
	Headphone jack 1 = 5

	Headphone jack 2 = GPIO 2

	Analog:
	Volume Pot = A1
	Station Pot = A2
	Amp SDA = A4
	Amp SCL = A5

****************************************************/
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

//Volume potentiometer
  int volPotPin = 1; //Volume input pin
  int8_t volVal = 0;

//Station potentiometer
  int stationPotPin = 2; //Station selection pin
  int stationVal = 0; //Default station number (off)

  int currentStation = 0;
  int lastStation = 0;

//Headphone and relay control pins
  int jackPin1 = 5;
  int jackPin2 = 2; //Need to change this since it references GPIO pins
  int speakerPin = 2;

  File root;

  File MANH;
  File BRONX;
  File QUEENS;
  File STATEN;
  File BKLN;

  int lastRandomStation = 0;


void setup() {
	//Init pins for speakers and headphones
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

	delay(500);
	//Print list of files
	root = SD.open("/listen/");
	printDirectory(root, 0);

  
  //Set volume
	volVal = map(analogRead(volPotPin), 0, 1023, 0, 63);
	setVolume(volVal);

	stationVal = analogRead(stationPotPin);
	Serial.println("StationVal:");
	Serial.println(stationVal);
	int station = getStationIDforPotValue(stationVal); 
	currentStation = station;

	// Timer interrupts are not suggested, better to use DREQ interrupt!
	//musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

	// If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
	// audio playing
	musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
	//Serial.println("Interupt pin ");

	// MusicPlayer GPIO pin as input for second jack
	//musicPlayer.GPIO_digitalWrite(1, HIGH);
	musicPlayer.GPIO_pinMode(jackPin2, INPUT);

	

	Serial.println("End setup");
}

void loop() {
	Serial.println("Start loop");
  delay(500);

	//Headphone relay testing
	int jack1 = digitalRead(jackPin1);
	//int jack2 = digitalRead(jackPin2);
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

	//digitalWrite(speakerPin, LOW);
	//Check volume
	int volValueRaw = analogRead(volPotPin);
	int computedVol = map(volValueRaw, 0, 1023, 0, 63);
	volVal = computedVol;
	setVolume(volVal);
	// Vol pot range is 0-1023
	// MAX9744 can be written from 0-64

	//Check station from pin 2
	stationVal = analogRead(stationPotPin);
	int station = getStationIDforPotValue(stationVal); 
	currentStation = station;
	Serial.println("Current Station:");
	Serial.println(currentStation);


	if(!musicPlayer.playingMusic || currentStation != lastStation) {
		musicPlayer.stopPlaying();

		if(station >= 6) {
			if(lastRandomStation != 0){
			    playFile(lastRandomStation);
			} else {
				lastRandomStation = playRandomStation();
				playFile(lastRandomStation);
			}       
		} else if (station != 0) {
			playFile(station);
			lastRandomStation = 0;
		} 
		lastStation = currentStation;
		delay(100);
   Serial.println();
	}
}
//END LOOP


int getStationIDforPotValue(int v) {
	int computedChannel;
	if (v > 0){
		computedChannel = map(v, 0, 1023, 1, 10);
	} else computedChannel = 0;
	return computedChannel;
} 

void playFile(int id) {
	if (id == 1) {
		root = SD.open("/listen/01");
		MANH = root.openNextFile();
	  Serial.println("Playing Manhattan");
	  musicPlayer.startPlayingFile(MANH.name());
	  root.close();
	  MANH.close();
	} 
	else if (id == 2) {
		root = SD.open("/listen/02/");
		QUEENS = root.openNextFile();
	  Serial.println("Playing Queens");
	  musicPlayer.startPlayingFile(QUEENS.name());
	  root.close();
	  QUEENS.close();
	}
	else if (id == 3) {
		root = SD.open("/listen/03/");
		BKLN = root.openNextFile();
	  Serial.println("Playing Brooklyn");
	  musicPlayer.startPlayingFile(BKLN.name());
	  root.close();
	  BKLN.close();
	}
	else if (id == 4) {
		root = SD.open("/listen/04/");
		BRONX = root.openNextFile();
	  Serial.println("Playing Bronx");
	  musicPlayer.startPlayingFile(BRONX.name());
	  root.close();
	  BRONX.close();
	}
	else if (id == 5) {
		root = SD.open("/listen/05/");
		STATEN = root.openNextFile();
	  Serial.println("Playing Staten");
	  musicPlayer.startPlayingFile(STATEN.name());
	  root.close();
	  STATEN.close();
	} 
}

int playRandomStation() {
	int randomStation = random(1, 5);
	Serial.println("Random station: " + randomStation);
	playFile(randomStation);
	currentStation = randomStation;
}

// File listing. Print list of files on SD card to serial. 
// Only called if SD card confirmed present.
void printDirectory(File dir, int numTabs) {
	while(true) {
	  
	  File entry =  dir.openNextFile();
	  if (! entry) {
	    // no more files
	    //Serial.println("**nomorefiles**");
	    break;
	  }
	  for (uint8_t i=0; i<numTabs; i++) {
	    Serial.print('\t');
	  }
	  Serial.print(entry.name());
	  if (entry.isDirectory()) {
	    Serial.println("/");
	    printDirectory(entry, numTabs+1);
	  } else {
	    // files have sizes, directories do not
	    //Serial.print("\t\t");
	    //Serial.println(entry.size(), DEC);
	  }
	  entry.close();
	}
	Serial.println();
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
