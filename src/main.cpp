/*
 *rst -9
 *sda - 10
 *mosi - 11
 *miso - 12
 *sck - 13
 *
 *van links naar rechts keys naar je toe ( * 0 #)
 *1 - leeg 
 *2 - 2
 *3 - 3
 *4 - 4
 *5 - 5 
 *6 - 6
 *7 - 7
 *8 - 8
 *leeg
 */

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {3, 8, 7, 5};
byte colPins[COLS] = {4, 2, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop(){
  //Serial.println("goedemorgen");
  ask_for_key();
  ask_for_UID();
  } 
  
void ask_for_UID(){
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID:");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println("");
}

void ask_for_key(){
  char customKey = customKeypad.getKey();
  if(customKey){
    Serial.print("key");
    Serial.print(customKey);
    Serial.println("");
    }
  }
