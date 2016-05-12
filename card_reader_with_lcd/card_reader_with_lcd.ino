/**
   ----------------------------------------------------------------------------
   This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
   for further details and other examples.

   NOTE: The library file MFRC522.h has a lot of useful info. Please read it.

   Released into the public domain.
   ----------------------------------------------------------------------------
   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

*/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN          5           // Configurable, see typical pin layout above
#define SS_PIN          53          // Configurable, see typical pin layout above
//uncomment this line if using a Common Anode LED
#define COMMON_ANODE

/////////////////////////////////////////////////////////////////////
//LCD Constants
/////////////////////////////////////////////////////////////////////
LiquidCrystal lcd(40,38,48,46,44,42);
const int switchPin = 6;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
/////////////////////////////////////////////////////////////////////
//set the pins
/////////////////////////////////////////////////////////////////////
// NEED TO BE PWM Pins to support analogWrite Function
const int LED_RED_PIN = 8;
const int LED_BLUE_PIN = 9;
const int LED_GREEN_PIN = 10;

const char SEPARATOR[] = "//";
String currentdeskId = "MB68-2-002";

// SEAT STATES
const int FREE = 0;
const int BUSY = 1;
const int RESERVED = 2;

////////////////////////
//Set the occuped state
boolean isBusy = false;
//set the userId
byte currentUser[18];
///////////////////////
//Maximum length of the array
#define MAX_LEN 16
//////////////////
//HELPER FUNCTIONS
//////////////////
void busy(byte *userId) {
  char *uId = (char*) userId;
  String str(uId);
  str = str.substring(0,7);
  
  
  for (int i = 0 ; i < 18; i++) {
    currentUser[i] = userId[i];
    uId[i] = userId[i];
  }
  isBusy = true;
  setColor(255, 0, 0);
  serialWrite(userId, BUSY);
  
    lcd.setCursor(0,1);
    lcd.print(str + "        ");
}
void releaseSeat(byte userId[]) {

  if (compareBuffers(currentUser, userId)) {
    isBusy = false;
    setColor(0, 255, 0);
    serialWrite(userId, FREE);
    
        lcd.setCursor(0,1);
        lcd.print("Desk frei!     ");
  }
  else {
    forbidden();
  }

}
void reserve() {
  setColor(255, 102, 0);
}

void forbidden() {
  setColor(255, 255, 255);
  delay(300);
  setColor(255, 0, 0);
  delay(300);
  setColor(255, 255, 255);
  delay(300);
  setColor(255, 0, 0);
}

void serialWrite(byte *userId, int state) {
  Serial.print("ARD_STATE");
  Serial.print(SEPARATOR);
  Serial.print(state);
  Serial.print(SEPARATOR);
  Serial.print(currentdeskId);
  Serial.print(SEPARATOR);
  for (byte i = 0; i < 16; i++) {
    Serial.print(userId[i] < 0x10 ? " 0" : " ");
    Serial.print(userId[i], HEX);
  }
  Serial.println("");
}


void setColor(int red, int green, int blue)
{
#ifdef COMMON_ANODE
  red = 255 - red;
  green = 255 - green;
  blue = 255 - blue;
#endif
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}


boolean compareBuffers(byte *buffer1, byte *buffer2) {
  if (sizeof(buffer1) != sizeof(buffer2)) {
    return false;
  }

  for (int i = 0 ; i < 18; i++) {
    boolean elemenEqual = buffer1[i] == buffer2[i];
    if (!elemenEqual) {
      return false;
    }
  }

  return true;
}

void logOutput(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
///////////////////////////////////////
void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  //releaseSeat();
  isBusy = false;
  setColor(0, 255, 0);

  // Setup LCD Display
  // put your setup code here, to run once:
  
    lcd.begin(16, 2);
    pinMode(switchPin, INPUT);
    lcd.print("Willkommen...");
    lcd.setCursor(0,1);
    lcd.print("Desk frei!");
}
void loop() {

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  // Show some details of the PICC (that is: the tag/card)
  Serial.print(F("Card UID:"));
  logOutput(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
          &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7
  byte sector         = 1;
  byte blockAddr      = 4;
  byte trailerBlock   = 7;

  MFRC522::StatusCode status;
  byte buffer[18];
  byte size = sizeof(buffer);

  // Authenticate using key A
  //Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // Show the whole sector as it currently is
  //Serial.println(F("Current data in sector:"));
  //mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
  //Serial.println();

  // Read data from the block
  //Serial.print(F("Reading data from block "));
  //Serial.print(blockAddr);
  //Serial.println(F(" ..."));
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  else {
    if (isBusy) {
      releaseSeat(buffer);
    }
    else {
      busy(buffer);
    }
    //logOutput(buffer, 16);
    //Serial.println();
    //Serial.println();
  }


  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}



