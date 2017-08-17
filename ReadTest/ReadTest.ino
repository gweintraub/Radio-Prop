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
	testFile = SD.open("text.txt");
	//if open then write
	if (testFile) {
		Serial.println("Test file open");
		//Read from file
		while(testFile.available()){
		    Serial.write(testFile.read());
		}
		//close file when out of lines to read
		testFile.close();
	} else {
		//Error if file didn't open
		Serial.println("Error opening file");
	}
}

void loop() {
	
}
