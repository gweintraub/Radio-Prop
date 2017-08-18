/*************************************************** 
	This Arduino class is made for a custom radio sound box
	Made by Gabe Weintraub for The Factory NYC, based on the original design by Rachel Ciavarella.

	gabe.weintraub@gmail.com
	rachel.ciavarella@gmail.com

	Relevant boards and libraries:
	Adafruit Micro SD breakout
	Adafruit MAX9744 breakout
	Adafruit VS1053 breakout board w/o amp

****************************************************/

//Pins
#define VS1053_DCS 7
#define VS1053_CS 6
#define CARDCS 4
#define VS1053_DREQ 3
#define VS1053_RESET 9

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Wire.h>

Adafruit_VS1053_FilePlayer musicPlayer = 
  // Create shield object
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//Volume potentiometer
  int volPotPin = 1; //Volume input pin
  int volVal = 0; //Volume range variable

//Station potentiometer
  int stationPotPin = 2; //Station selection pin
  int stationVal = 0; //Default station number (off)

  int currentStation;
  int lastStation;

//Headphone and relay control pins
  int jackPin1 = 5;
  int jackPin2 = 2; //Need to change this since it references GPIO pins
  int speakerPin = 2;
  pinMode(jackPin1, INPUT);
  pinMode(jackPin2, INPUT);
  pinMode(speakerPin, OUTPUT);

void setup() {
	Serial.begin(9600);

	//Initialize musicPlayer board
	Serial.println("Initializing VS1053 board...");
	if(! musicPlayer.begin()) {
	    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
	    while(1);
	}
	Serial.println(F("VS1053 found"));
	
	//Initialize MicroSD card
	Serial.println("Initializing Micro SD card...");
	if(!SD.begin(CARDCS)) {
	    Serial.println("Initialization failed! Check SD card.");
	    while(1); //end
	} 
	Serial.println("Initialization complete");
	//End initialization

	//Print list of files
	printDirectory(SD.open("/"), 0);
	// Set volume for left, right channels. 
	// Lower numbers == louder volume!
	musicPlayer.setVolume(50,50);

	// Timer interrupts are not suggested, better to use DREQ interrupt!
	//musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

	// If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
	// audio playing
	musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

	// MusicPlayer GPIO pin as input for second jack
	//musicPlayer.GPIO_digitalWrite(1, HIGH);
	musicPlayer.GPIO_pinMode(jackPin2, INPUT);
}

void loop() {
	Serial.println("Start loop");

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

	//Check volume
	//NEED TO MODIFY VOLUME CODE FOR DIGITAL CONTROL VIA MAX9744 I2C
	//CONVERT P
	int volValue = analogRead(volPotPin);
	/*************************
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	THIS IS SO I DON'T FORGET TO FIX THE VOLUME CODE
	***************************/

	//Check station from pin 1
	int stationValue = analogRead(stationPotPin);
	Serial.println("stationValue = " + stationValue);

	int station = getStationIDforPotValue(stationValue); //stationValue/100);
	Serial.println("Station = " + station);

	currentStation = station;


	if(!musicPlayer.playingMusic || currentStation != lastStation) {
		musicPlayer.stopPlaying();

		if(station == 6) {
			playRandomStation();	        
		} else if (station != 0) {
			playFile(station);
		} 
		lastStation = currentStation;
		delay(100);
	}
	musicPlayer.setVolume(volume,volume); //THIS MAY NEED SOME FIXING
}
//END LOOP


int getStationIDforPotValue(int v) {
	if(v < 30 || v > 650) {
		return 0; //OFF
	} else if (v > 30 && v < 50) {
		return 6; //Random station
	} else if (v > 140 && v < 160) {
    	return 5;
  	} else if (v > 260 && v < 280) {
    	return 4;
  	} else if (v > 380 && v < 400) {
    	return 3;
  	} else if (v > 500 && v < 520) {
    	return 2;
  	} else if (v > 610 && v < 630) {
    	return 1;
  	}
} 

void playFile(int id) {
	if (id == 1) {
	  Serial.println("playing 1");
	  musicPlayer.startPlayingFile("1.mp3");
	} 
	else if (id == 2) {
	  Serial.println("playing 2");
	  musicPlayer.startPlayingFile("2.mp3");
	}
	else if (id == 3) {
	  Serial.println("playing 3");
	  musicPlayer.startPlayingFile("3.mp3");
	}
	else if (id == 4) {
	  Serial.println("playing 4");
	  musicPlayer.startPlayingFile("4.mp3");
	}
	else if (id == 5) {
	  Serial.println("playing 5");
	  musicPlayer.startPlayingFile("5.mp3");
	} 
}

void playRandomStation() {
	int randomStation = random(1, 5);
	Serial.println("Random station: " + randomStation);
	playFile(randomStation);
}

// File listing. Print list of files on SD card to serial. 
// Only called if SD card confirmed present.
void printDirectory(File dir, int numTabs) {

	while(true){
	    File entry = dir.openNextFile(); // Find first file on SD
	    if(! entry){  // If no more files
	        Serial.println("**nomorefiles**");
	        break;
	    }
	    for(uint8_t i=0; i<numTabs; i++){ // Why did rachel print tabs here?
	        Serial.print('\t'); 
	    }
	    Serial.println(entry.name());
	    // if(entry.isDirectory()) {
	    // 	Serial.println("/");
	    // 	printDirectory(entry, numTabs+1);   
	    // } else {
	    // 	//Files have sizes, directories do not
	    // 	Serial.print("\t\t" );
	    // 	Serial.println(entry.size(), DEC);
	    // }
	    entry.close();
	}
}
