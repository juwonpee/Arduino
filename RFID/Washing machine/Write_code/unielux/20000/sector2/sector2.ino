/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

MFRC522::MIFARE_Key key;
MFRC522::MIFARE_Key key2;

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();               // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("Write dump"));

  // Prepare key
  byte keys[] = {
    0x6B, 0x73, 0x63, 0x6F, //66 77 63 6f
    0x69, 0x6E,
  };
  // for my sanity
  for (int i = 0; i < 6; i++) {
    key.keyByte[i] = keys[i];
  }
  //second key
  for (byte i = 0; i < 6; i++) {
    key2.keyByte[i] = 0xFF;
  }
}

  byte blockAddr8 = 8;
  byte blockAddr9 = 9;
  byte blockAddr10 = 10;
  byte blockAddr11 = 11;


//Block 8, type A, key 6677636f696e :88 02 00 00 / 00 00 FF 07 / 80 69 FF FF / FF FF FF FF 
  byte dataBlock8[]    = {
    0x88, 0x02, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0x07, 
    0x80, 0x69, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF,
  };
//Block 9, type A, key 6677636f696e :88 02 00 00 / 00 00 FF 07 / 80 69 FF FF / FF FF FF FF 
  byte dataBlock9[]    = {
    0x88, 0x02, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0x07, 
    0x80, 0x69, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF,
  };
//Block 10, type A, key 6677636f696e :88 02 00 00 / 00 00 FF 07 / 80 69 FF FF / FF FF FF FF    
  byte dataBlock10[]    = {
    0x88, 0x02, 0x00, 0x00, 
    0x00, 0x00, 0xFF, 0x07, 
    0x80, 0x69, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF,
  };
//Block 11, type A, key 6677636f696e :00 00 00 00 / 00 00 FF 07 / 80 69 FF FF / FF FF FF FF
  byte dataBlock11[]    = {
    0x6B, 0x73, 0x63, 0x6F, //66 77 63 6f
    0x69, 0x6E, 0xFF, 0x07, //69 6e
    0x80, 0x69, 0x00, 0xFF,  
    0xFF, 0xFF, 0xFF, 0xFF,
  };

void loop() {
    // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  
    // Dump UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.print(F(" PICC type: "));   // Dump PICC type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));



  byte sector = 2;
  byte block = sector * 4 + 3;
  MFRC522::StatusCode status;
  //authenticating with keys
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  Serial.println(MFRC522::STATUS_OK);
  Serial.println(status);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    //return;
    delay(200000);
  }


  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  
  //authenticate again with key b
//  Serial.println(F("Authenticating again using key B..."));
//  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key2, &(mfrc522.uid));
//  Serial.println(MFRC522::STATUS_OK);
//  Serial.println(status);
//  if (status != MFRC522::STATUS_OK) {
//    Serial.print(F("PCD_Authenticate() failed: "));
//    Serial.println(mfrc522.GetStatusCodeName(status));
//    //return;
//    delay(200000);
//  }


  //writing to block 8
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr8);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr8, dataBlock8, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 9
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr9);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr9, dataBlock9, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 10
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr10);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr10, dataBlock10, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 11M
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr11);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr11, dataBlock11, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println("Sector 2 copy complete");


  
  Serial.println();
  delay(200000);
}
