/*
  Listfiles

 This example shows how print out the files in a
 directory on a SD card

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 modified 2 Feb 2014
 by Scott Fitzgerald

 This example code is in the public domain.

 */
#include <SPI.h>
#include <SD.h>

#define NUMFOLDERS 5

File root;
int folders[NUMFOLDERS];

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //Serial.println(sizeof(folders));
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  root = SD.open("/listen/");

  printDirectory(root, 0);

  Serial.println("done!");

  for(int i=0; i<sizeof(folders); i++){
//      Serial.print(i);
//      Serial.print(":");
//      Serial.println(folders[i]);
  }
}

void loop() {
  // nothing happens after setup finishes.
}

void printDirectory(File dir, int numTabs) {
  for(int i=0; i<NUMFOLDERS; i++){
      while (true) {
        File entry =  dir.openNextFile();
        if (! entry) {
          // no more files
          break;
        }
        if (entry.isDirectory()) {
          //Serial.println(entry.name());
          countFiles(entry, i);
          i++;
        }
        entry.close();
      }
  }
}

void countFiles(File dir, int index) {
  int numFiles = 0;
  while(true){
      File entry = dir.openNextFile();
      if(! entry){
        break;
      }
      numFiles++;
      entry.close();
  } 
  folders[index] = numFiles;
  Serial.println(folders[index]);
}


