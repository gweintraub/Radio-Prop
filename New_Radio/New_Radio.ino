/*************************************************** 
	This Arduino class is made for a custom radio sound box
	Made by Gabe Weintraub for The Factory NYC, based on the original design by Rachel Ciavarella.

	gabe.weintraub@gmail.com
	rachel.ciavarella@gmail.com

	Relevant boards and libraries:
	Adafruit Micro SD breakout
	Adafruit MAX9744 breakout

****************************************************/

#include <SPI.h>
#include <SD.h>

void setup() {
	Serial.begin(9600);
	Serial.println("Initializing Micro SD card...");
	pinMode(10, OUTPUT);

	if(!SD.begin(10)) {
	    Serial.println("Initialization failed! Check SD card.");
	    return;
	} Serial.println("Initialization complete");
	//End initialization

	//Print list of files
	printDirectory(SD.open("/"), 0);
}

void loop() {
	
}

int getStationIDforPotValue(int v) {

}

void playFile(int id) {

}

void playRandomStation() {

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
