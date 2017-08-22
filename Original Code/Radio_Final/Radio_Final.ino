/*************************************************** 
  This is code for custom sound box radio 
  Made by Rachel Ciavarella for The Factory
  rachel.ciavarella@gmail.com
  
  Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Library by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
//#include <random>

// define the pins used
//#define CLK 13       // SPI Clock, shared with SD card
//#define MISO 12      // Input data, from VS1053/SD card
//#define MOSI 11      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins. 
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = 
  // create shield-example object!
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//setting up the volume knob
  int volPotPin = 1;    // select the input pin for the potentiometer
  int volVal = 0;       // variable to store the value coming from the sensor

//setting up the station knob
  int stationPotPin = 2;    // select the input pin for the potentiometer
  int stationVal = 0;       // variable to store the value coming from the sensor

//setting up the current and last station variables
  int currentStation;
  int lastStation;

//set up the crazy headphone + speaker situation
  int jackPin1 = 5;
  int jackPin2 = 2; 
  int speakerPin = 2;

void setup() {


//more speaker + heaphone set up
pinMode(jackPin1, INPUT);
// pinMode(jackPin2, INPUT);
pinMode(speakerPin, OUTPUT);



  Serial.begin(9600);
//  Serial.println("Adafruit VS1053 Simple Test");

  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
   if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(50,50);

 
  // Timer interrupts are not suggested, better to use DREQ interrupt!
  //musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
Serial.println("thisistheendofsetup");

//setup musicPlayer gpio pin as input for second jack
//musicPlayer.GPIO_digitalWrite(1, HIGH);
musicPlayer.GPIO_pinMode(jackPin2, INPUT);


}

void loop() {
//Serial.println("startloop");

//set digital pins for headphone jacks and speaker
int jack1 = digitalRead(jackPin1);
//int jack2 = digitalRead(jackPin2);
int jack2 =  musicPlayer.GPIO_digitalRead(jackPin2);

//When headphones are plugged in it opens switch and sets jack pins to LOW
//if jack1 or jack2 is low (plugged in) make speakerpin HIGH (this flips relay from closed to open)
if (jack1 == LOW || jack2 == LOW) {
  digitalWrite(speakerPin, HIGH);
//  Serial.println("jack plugged in / speaker off");
}
//else if jacks aren't LOW speakerpin should be LOW (relay set to closed)
else {
  digitalWrite(speakerPin, LOW);
//  Serial.println("jack unplugged / speaker on");
}


  
// read the input on analog pin 0:
  int volValue = analogRead(volPotPin);
  // print out the value you read:
//  Serial.println(volValue);  

//doing math to set volume based on volume pot value
  int volume = (volValue / 10);
//  Serial.println(volume);

     // read the input on analog pin 1:
  int stationValue = analogRead(stationPotPin);
  // print out the value you read:
  Serial.println(stationValue);  

  int station = getStationIdForPotValue(stationValue); //(stationValue / 100);
  Serial.println(station);

  currentStation = station;

 if (!musicPlayer.playingMusic || currentStation != lastStation) {

  musicPlayer.stopPlaying();
  
  if (station == 6) {
    playRandomStation();
  } 
  else if (station != 0) {
    playFile(station);
  }
  
  lastStation = currentStation;
  delay(100);
 }

  musicPlayer.setVolume(volume,volume);
  // File is playing in the background

  delay(100);
}

//END OF LOOP

int getStationIdForPotValue(int v){
  if(v < 30 || v >650){
    return 0; //OFF
  }
  else if (v > 30 && v < 50) {
    return 6;//RANDOM USED TO BE STATION 1 REVERSED AT SHOP 
  } 
  else if (v > 140 && v < 160) {
    return 5;
  }
  else if (v > 260 && v < 280) {
    return 4;
  }
  else if (v > 380 && v < 400) {
    return 3;
  }
  else if (v > 500 && v < 520) {
    return 2;
  }
  else if (v > 610 && v < 630) {
    return 1; //USED TO BE RANDOM REVERESED ORDER WHILE AT SHOP
  }
}

void playFile(int id){
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

void playRandomStation(){
  int randomStation = random(1,5);
  String R = "Random Station:";
  Serial.println(R + randomStation);
  playFile(randomStation);
}




/// File listing helper
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
//     if (entry.isDirectory()) {
       Serial.println("/");
//       printDirectory(entry, numTabs+1);
//     } else {
//       // files have sizes, directories do not
//       Serial.print("\t\t");
//       Serial.println(entry.size(), DEC);
//     }
     entry.close();
   }
}

