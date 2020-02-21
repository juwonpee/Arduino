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
    0x66, 0x77, 0x63, 0x6F, //6677636f696e
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

  byte blockAddr16 = 16;
  byte blockAddr17 = 17;
  byte blockAddr18 = 18;
  byte blockAddr19 = 19;
//Block 16, type A, key 6677636f696e :00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00 
  byte dataBlock16[]    = {
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
  };
//Block 17, type A, key 6677636f696e :88  0b  16  21  00  00  ff  07  80  69  ff  ff  ff  ff  ff  ff  
  byte dataBlock17[]    = {
    0x88, 0x0B, 0x16, 0x21, 
    0x00, 0x00, 0xFF, 0x07, 
    0x80, 0x69, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF,
  };
//Block 18, type A, key 6677636f696e :00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00
  byte dataBlock18[]    = {
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 
  };
//Block 19, type A, key 6677636f696e :00  00  00  00  00  00  ff  07  80  69  ff  ff  ff  ff  ff  ff   sector 5
  byte dataBlock19[]    = {
    0x66, 0x77, 0x63, 0x6F, //66 77 63 6f
    0x69, 0x6E, 0xFF, 0x07, //69 6e
    0x80, 0x69, 0xFF, 0xFF,  
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


  byte sector = 4;
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
    return;
  }


  // Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  Serial.println();

  
  //authenticate again with key b
  Serial.println(F("Authenticating again using key B..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, block, &key2, &(mfrc522.uid));
  Serial.println(MFRC522::STATUS_OK);
  Serial.println(status);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }


  //writing to block 16
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr16);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr16, dataBlock16, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 17
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr17);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr17, dataBlock18, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 18
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr18);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr18, dataBlock18, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println();
  
  //writing to block 19
  Serial.print(F("Writing data into block ")); Serial.print(blockAddr19);
  Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr19, dataBlock19, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  Serial.println("Sector 3 copy complete");


  
  Serial.println();
  delay(2000);
}
