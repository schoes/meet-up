#include <LiquidCrystal.h>

LiquidCrystal lcd(8,7,4,2,1,0);
const int switchPin = 6;
int switchState = 0;
int prevSwitchState = 0;
int reply;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(switchPin, INPUT);
  lcd.print("Willkommen...");
  lcd.setCursor(0,1);
  lcd.print("Freies Desk");
}

void loop() {
  // put your main code here, to run repeatedly:

}
