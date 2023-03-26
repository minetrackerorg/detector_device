////////////////////////////////////////////////////////////
// Basic demo for readings from Adafruit BNO08x
#include <Adafruit_BNO08x.h>

// For SPI mode, we need a CS pin
#define BNO08X_CS 10
#define BNO08X_INT 9

// For SPI mode, we also need a RESET 
//#define BNO08X_RESET 5
// but not for I2C or UART
#define BNO08X_RESET -1

Adafruit_BNO08x  bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

bool writingFailed = false;
bool appendFailed = false;
//////////////////////////////////////////////////////////

/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
#include "FS.h"
#include "SD.h"
#include "SPI.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

//void writeFile(fs::FS &fs, const char * path, const char * message){
void writeFile(fs::FS &fs, const char * path, const String message){
    //Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written: ");
        Serial.println(message);
        writingFailed = false;
    } else {
        Serial.println("Write failed");
        writingFailed = true;
    }
    file.close();
}

//void appendFile(fs::FS &fs, const char * path, const char * message){
void appendFile(fs::FS &fs, const char * path, const String message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
        appendFailed = false;
    } else {
        Serial.println("Append failed");
        appendFailed = true;
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////START
// Here is where you define the sensor outputs you want to receive
void setReports(void) {
  Serial.println("Setting desired reports");
  if (! bno08x.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED)) {
    Serial.println("Could not enable magnetic field calibrated");
  }
  if (!bno08x.enableReport(SH2_RAW_MAGNETOMETER)) {
    Serial.println("Could not enable raw magnetometer");
  }
}

String sendReport(){
  String content = "";
  switch (sensorValue.sensorId) {
      
      //case SH2_MAGNETIC_FIELD_CALIBRATED:
        //Serial.print("Magnetic Field - x: ");
        //Serial.print(sensorValue.un.magneticField.x);
        //Serial.print(" y: ");
        //Serial.print(sensorValue.un.magneticField.y);
        //Serial.print(" z: ");
        //Serial.println(sensorValue.un.magneticField.z);
        //content = "Magnetic Field - x: " + String(sensorValue.un.magneticField.x) + " y: " + String(sensorValue.un.magneticField.y) + " z: " + String(sensorValue.un.magneticField.z);
        //break;
      case SH2_RAW_MAGNETOMETER:
        Serial.print("Raw Magnetic Field - x: ");
        Serial.print(sensorValue.un.rawMagnetometer.x);
        Serial.print(" y: ");
        Serial.print(sensorValue.un.rawMagnetometer.y);
        Serial.print(" z: ");
        Serial.println(sensorValue.un.rawMagnetometer.z);
        content = "Magnetic Field - x: " + String(sensorValue.un.magneticField.x) + " y: " + String(sensorValue.un.magneticField.y) + " z: " + String(sensorValue.un.magneticField.z);
        break;
    }
    return content;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////END

void setup(){
      Serial.begin(115200);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////START
      while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens
    
      Serial.println("Adafruit BNO08x test!");
    
      // Try to initialize!
      if (!bno08x.begin_I2C()) {
      //if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte UART buffer!
      //if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println("Failed to find BNO08x chip");
        while (1) { delay(10); }
      }
      Serial.println("BNO08x Found!");
    
      for (int n = 0; n < bno08x.prodIds.numEntries; n++) {
        Serial.print("Part ");
        Serial.print(bno08x.prodIds.entry[n].swPartNumber);
        Serial.print(": Version :");
        Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
        Serial.print(".");
        Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
        Serial.print(" Build ");
        Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
      }
    
      setReports();
    
      Serial.println("Reading events");
      delay(100);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////END
    //Serial.begin(115200);
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);
    //createDir(SD, "/mydir");
    //listDir(SD, "/", 0);
    //removeDir(SD, "/mydir");
    //listDir(SD, "/", 2);
    //writeFile(SD, "/hello.txt", "Hello ");
    //appendFile(SD, "/hello.txt", "World!\n");
    //readFile(SD, "/hello.txt");
    //deleteFile(SD, "/foo.txt");
    //renameFile(SD, "/hello.txt", "/foo.txt");
    //readFile(SD, "/foo.txt");
    //testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop(){
    delay(100);

    if (bno08x.wasReset()) {
      Serial.print("sensor was reset ");
      setReports();
    }
    
    if (! bno08x.getSensorEvent(&sensorValue)) {
      return;
    }
    
    
  
    if (Serial.available() > 0) { // check if there is incoming data
      String input = Serial.readStringUntil('\n'); // read the input string until a newline character is received
      if (input == "DIR") { // check if the input string matches "DOIT"
        listDir(SD, "/Cassini_Hackathon_Data", 0); // call the doit() function
      }
      if (input == "START") {
        sendReport();
      }
      if (input == "WRITE") { 
        Serial.print("Writing to file: ");
        //char* content = (char*)sendReport().c_str();
        String content = sendReport();
        if(content != ""){
          content = content + "\n";
          //Serial.print(content);
          //Serial.print("\n");
          writeFile(SD, "/Cassini_Hackathon_Data/test.txt", content);
          while(writingFailed){
           writeFile(SD, "/Cassini_Hackathon_Data/test.txt", content);
          }
        }
        else{
          Serial.println("Empty message");
        }
      }
      if (input == "APPEND") { 
        Serial.print("Appending to file: ");
        //char* content = (char*)sendReport().c_str();
        String content = sendReport();
        if(content != ""){
          content = content + "\n";
          //Serial.print(content);
          //Serial.print("\n");
          appendFile(SD, "/Cassini_Hackathon_Data/test.txt", content);
          while(appendFailed){
           appendFile(SD, "/Cassini_Hackathon_Data/test.txt", content);
          }
        }else{
          Serial.println("Empty message");
        }
      }
      if (input == "READ") { 
        //Serial.println("Reading from file: \n");
        readFile(SD, "/Cassini_Hackathon_Data/test.txt");
      }
    }
}
