#include <AddicoreRFID.h>

#include <SPI.h>
#define uchar unsigned char
#define uint unsigned int
//4 bytes tag serial number, the first 5 bytes for the checksum byte
uchar serNumA[5];
//© Addicore LLC 2015 v1.2
uchar fifobytes;
uchar fifoValue;
AddicoreRFID myRFID; // create AddicoreRFID object to control the RFID module
/////////////////////////////////////////////////////////////////////
//set the pins
/////////////////////////////////////////////////////////////////////
const int chipSelectPin = 10;
const int NRSTPD = 5;
const int LED_RED_PIN = 2;
const int LED_GREEN_PIN = 3;
////////////////////////
//Set the occuped state
///////////////////////
//Maximum length of the array
#define MAX_LEN 16
//////////////////
//HELPER FUNCTIONS
//////////////////
void wirteOccupiedState(boolean occupied) {
  if (occupied) {
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(LED_GREEN_PIN, LOW);
  }
  else {
    digitalWrite(LED_GREEN_PIN, HIGH);
    digitalWrite(LED_RED_PIN, LOW);
  }
}

void setup() {
  Serial.begin(9600); // RFID reader SOUT pin connected to Serial RX pin at 9600bps
  // start the SPI library:
  SPI.begin();
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(chipSelectPin, OUTPUT); // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin
  digitalWrite(chipSelectPin, LOW); // Activate the RFID reader
  pinMode(NRSTPD, OUTPUT); // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);
  myRFID.AddicoreRFID_Init();
  wirteOccupiedState(false);
}
void loop()
{
  uchar i, tmp, checksum1;
  uchar status;
  uchar str[MAX_LEN];
  uchar RC_size;
  uchar blockAddr; //Selection operation block address 0 to 63
  String mynum = "";
  str[1] = 0x4400;
  //Find tags, return tag type
  status = myRFID.AddicoreRFID_Request(PICC_REQIDL, str);
  if (status == MI_OK)
  {
    Serial.println("RFID tag detected");
    Serial.print(str[0], BIN);
    Serial.print(" , ");
    Serial.print(str[1], BIN);
    Serial.println(" ");
  }
  //© Addicore LLC 2015 v1.2
  //Anti-collision, return tag serial number 4 bytes
  status = myRFID.AddicoreRFID_Anticoll(str);
  if (status == MI_OK)
  {
    checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
    Serial.println("The tag's number is : ");
    //Serial.print(2);
    Serial.print(str[0]);
    Serial.print(" , ");
    Serial.print(str[1], BIN);
    Serial.print(" , ");
    Serial.print(str[2], BIN);
    Serial.print(" , ");
    Serial.print(str[3], BIN);
    Serial.print(" , ");
    Serial.print(str[4], BIN);
    Serial.print(" , ");
    Serial.println(checksum1, BIN);

    // Should really check all pairs, but for now we'll just use the first
    if (str[0] == 156) //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
    {
      Serial.print("Hello Craig!\n");
    } else if (str[0] == 244) { //You can change this to the first byte of your tag by finding the card's ID through the Serial Monitor
      Serial.print("Hello Erin!\n");
    }
    Serial.println();
    delay(1000);
  }
  myRFID.AddicoreRFID_Halt(); //Command tag into hibernation
}



