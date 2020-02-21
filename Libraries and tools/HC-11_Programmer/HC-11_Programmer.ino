/* HC-11 433Mhz Programmer
 * written my Mel Patrick
 * Wabbit Wanch Design
 * Connect CON to pin 4 (only used for programming)
 * Connect TX to Pin 2
 * Connect RX to Pin 3
 * Vcc to +5V
 */
//
#include <SoftwareSerial.h>
#include <TextFinder.h>

#define hcSetPin 4
SoftwareSerial hcSerial(2, 3); // RX, TX
long baudRateTable[] = {
  1200,2400,4800,9600,19200,38400,57600,115200};
int8_t baudEntries=8;
long theBaudSetting = 0;
boolean baudFound = false;
char inBuffer[20];
int inBuffPosition = 0;
char hcBuffer[20];
int hcBuffPosition=0;
char commandSetBuffer[30];
char hcMode[] = "9";
char hcBaud[] = "230300";
char hcChannel[] = "999";
char hcAddress[] = "999";
char hcPower[] = "0";
char hcConfig[50];// holds the config when the HC updates
//
TextFinder finder(hcSerial,2);// for parsing the results from the module
//
boolean getHCSettings = false;
//
void setup() {
  Serial.begin(9600);// this is for talking to the IDE monitor
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  digitalWrite(hcSetPin, HIGH);// test control the SET pin on the HC 11 for command mode
  pinMode(hcSetPin, OUTPUT);
  testBaudRate();// go find the baud rate the HC11 was programmmed with
  //showHeading();// menu options to pick from
}

void loop() {
  grabKeystrokes();// use the IDE Serial monitor to program the HC-11
  handleHCAnswer();// parse the reply that comes back from the HC-11
}
//
void handleHCAnswer() {
  if (hcSerial.available()) {
    char theKey = hcSerial.read();
    if (hcBuffPosition != 0) {
      hcBuffer[hcBuffPosition] = theKey;
      hcBuffPosition++;
      if (theKey == '\n') {
        if (getHCSettings == true) {
          handleHCSettingsReply();// get extract all the data
        }
        else
        {
          if (hcBuffer[0]=='O' && hcBuffer[1]=='K'){
            askHCConfig();
          }
          else
          {
            Serial.print(hcBuffer);
          }
        }
        hcBuffPosition=0;
        bufferClear();
      }
    }
    else
    {
      if (theKey != '\r' || theKey != '\n' ) {
        hcBuffer[hcBuffPosition] = theKey;
        hcBuffPosition++;
      }
    }
  }
}
//
void handleHCSettingsReply() {
  int pos=0;
  switch (hcBuffer[0]) {
  case 'U':
    hcMode[0] = hcBuffer[1];// copy to my buffer
    //Serial.println(hcBuffer);
    break;
  case 'B':
    while (hcBuffer[pos+1] != '\r') {
      hcBaud[pos] = hcBuffer[pos+1];
      pos++;
    }
    hcBaud[pos]='\0';
    break;
  case 'C':
    for (int pos=0;pos < 3;pos++) {
      hcChannel[pos] = hcBuffer[pos+1];
    }
    break;
  case 'A':
    for (int pos=0;pos < 3;pos++) {
      hcAddress[pos] = hcBuffer[pos+1];
    }
    break;
  case 'P':
    hcPower[0] = hcBuffer[1];
    getHCSettings = false;
    sprintf(hcConfig,"Mode:%s Baud:%s Chan:%s Add:%s Pow:%s\0",hcMode,hcBaud,hcChannel,hcAddress,hcPower);
    showHeading();// menu options to pick from
    break;
  }
}
void grabKeystrokes() {
  while (Serial.available()) {
    char theKey = Serial.read();// read the characters you type
    inBuffer[inBuffPosition] = theKey;// save it in the buffer
    inBuffPosition++;// point to the next buffer position
    if (inBuffPosition > 15) inBuffPosition=15;// so we don't overrun memory
    if (theKey == '\n') {
      inBuffPosition--;
      inBuffPosition--;
      if (inBuffPosition > 0) {// did we actually type in a command?
        inBuffer[inBuffPosition] = '\0';// a zero is a C EOL
        testForCommands();
      }
      inBuffPosition=0;// reset for end
    }
  }
}
//
void showHeading() {
  Serial.println(F("-----------------------------------------"));
  Serial.println(F("HC-11 Programmer by Wabbit Wanch Design"));
  Serial.println(hcConfig);
  Serial.println(F("-----------Check Commands----------------"));
  Serial.println(F("CM - Menu             CV - Firmware"));
  Serial.println(F("------------Set Commands-----------------"));
  Serial.println(F("SA - Address   (SA=000 - 255)"));
  Serial.println(F("SB - Baud Rate (SB=1200 - 115200)"));
  Serial.println(F("SC - Channel   (SC=000 - 020)"));
  Serial.println(F("SM - Mode      (SM=1 - 4)"));
  Serial.println(F("SP - Power     (SP=1 - 8)"));
  Serial.println(F("Command:"));
}
//
void testForCommands() {
  strupr(inBuffer);// convert the buffer to upper case
  switch (inBuffer[0]) {
  case 'C':
    switch (inBuffer[1]) {
    case 'V':
      handleFMcheck();
      break;
    case 'M':
      showHeading();
      break;
    }
    break;
  case 'S':
    switch (inBuffer[1]) {
    case 'A':
      setAddress();
      break;
    case 'B':
      setNewBaud();
      break;
    case 'C':
      setNewChannel();
      break;
    case 'M':
      setNewMode();
      break;
    case 'P':
      setNewPower();
      break;
    }
    break;
  }
}
//
void setAddress() {
  extractnSetHC("AT+A");
}
//
void setNewBaud() {
  extractnSetHC("AT+B");
  digitalWrite(hcSetPin, HIGH);// test control the SET pin on the HC 11 for command mode
  delay(500);
  bufferClear();
  software_Reset();
}
void setNewChannel() {
  extractnSetHC("AT+C");
}
//
void setNewPower() {
  extractnSetHC("AT+P");
}
//
void setNewMode() {
  extractnSetHC("AT+FU");
}
//
void extractnSetHC(char theCMD[]) {
  char temp[30];
  int bufPos = 0;
  do {
    commandSetBuffer[bufPos] = inBuffer[3+bufPos];
    bufPos++;
  } 
  while (commandSetBuffer[bufPos-1] != '\0');
  bufPos--;// subtract one for the delimiter
  //Serial.println(bufPos);
  //Serial.println(commandSetBuffer);
  sprintf(temp,"%s%s\0", theCMD, commandSetBuffer);
  //Serial.println(temp);
  hcSerial.println(temp);// send to the HC-11
  bufferClear();// get rid of the junk from the HC
}
//
void bufferClear() {
  for (int myVal=0; myVal< 30; myVal++) {
    hcBuffer[myVal]='\0';
    inBuffer[myVal]='\0';
    commandSetBuffer[myVal]='\0';
  }
}
//
void handleFMcheck() {
  Serial.println();
  Serial.print(F("Firmware Version: "));
  hcSerial.println(F("AT+V"));
  delay(500);
}
//
void testBaudRate() {
  int testPoint = 0;
  do {
    Serial.print(F("Testing Baud Rate: "));
    Serial.println(baudRateTable[testPoint]);
    hcSerial.begin(baudRateTable[testPoint]);// this is for talking to the HC-11
    digitalWrite(hcSetPin, LOW);
    delay(100);
    hcSerial.print(F("AT\r\n"));// send AT command to the remote
    delay(100);
    if(finder.find("OK")) {
      baudFound = true;
      theBaudSetting = baudRateTable[testPoint];
      //Serial.print(theBaudSetting);
    }
    else
    {
      testPoint++;// advance to next baud rate
      if (testPoint == baudEntries) {
        theBaudSetting = 99999;
        baudFound = true;
      }
    }
  } 
  while (baudFound == false);
  if (theBaudSetting == 99999) {
    Serial.println(F("Unable to find baud rate! Stopping"));
    while (baudFound == true) {// just loop here for ever
    }
  }
  askHCConfig();
}
//
void askHCConfig() {
  bufferClear();
  getHCSettings = true;// process the return
  hcSerial.println(F("AT+RX"));
}
//
void software_Reset()// only used if you reset the baud rate, only tested on UNO
// Restarts program from beginning but 
// does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");  
}
