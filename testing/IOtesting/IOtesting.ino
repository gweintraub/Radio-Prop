/****************
File reading test for SD card board
*****************/

#include <SPI.h>
#include <SD.h>


File testFile;

void setup() {
	Serial.begin(9600);
	Serial.println("Initializing Micro SD card...");
	pinMode(10, OUTPUT);

	if(!SD.begin(10)) {
	    Serial.println("Initialization failed! Check SD card.");
	    return;
	} Serial.println("Initialization complete");
	//End initialization

	//Open test file
	testFile = SD.open("text.txt", FILE_WRITE);
	//if open then write
	if (testFile) {
		Serial.println("Writing to test file...");
		testFile.println("Test 1 2 3");
		testFile.close();
		Serial.println("Write compelte");
	} else {
		//Error if file didn't open
		Serial.println("Error opening file");
	}
}

void loop() {
	
}
