#define DEBUG

#include <SPI.h>
#include <MFRC522.h>

// GLOBALS
const byte numbReaders = 2;

const byte ssPins[] = {5, 17}  // Cambiar pines para ESP32

// They will share the same reset pin
const byte resetPin = 8;

MFRC522 mfrc522[numbReaders];

const String IDs = {"aaa", "bbb"}

String currentIDs[numbReaders]


void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG
  Serial.begin(9600);
  Serial.println(F("Serial communication started"));
  #endif

  // Initialise the SPI bus.
  SPI.begin();

  for (uint8_t i=0; i < numbReaders; i++)
  {
    mfrc522[i].PCD_Init(ssPins[i], resetPin);

    Serial.print(F("READER # "));
    Serial.print(i);
    Serial.print(F(" initialised  on pin "));
    Serial.print(String(ssPins[i]));

    delay(100)
     
  }

  Serial.println(F("----- END SETUP -----"));
  
}

void loop() {
  // put your main code here, to run repeatedly:

  // Loop through each reader
  for (uint8_t i=0; i < numbReaders; i++)
  {
    // Initialise the sensor
    mfrc522[i].PCD_Init();  

    // String to hold the ID detected by each sensor..
    String readRFID = "";

    // If the sensor detects  a tag and is able to read it...
    if(mfrc522[i].PICC_IsNewCardPresent() && mfrcs522[i].PICC_ReadCardSerial())
    { 
      // Extract the ID from the tag..
      readRFID = dump_byte_array(mfrc[i].uid.uidByte, mfrc522[i].uid.size);  
    }

   /* if(readRFID != currentIDs[i])
    {

      
      // Set flag to show that 
    }*/


    /// HATL PIC
    mfrc522[i].PICC_HaltA();
    // Stop encryption on PCD
    mfrc522[i].PCD_StopCrypto1();
  }

}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
