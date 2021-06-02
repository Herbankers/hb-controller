/*
 *rfid
 *rst -9
 *sda - 10
 *mosi - 11
 *miso - 12
 *sck - 13
 *
 *keypad
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
 *
 *bonprinter
 *vh - power van power supply
 *dtr - ground arduino
 *tx  - A1
 *rx - A0
 *gnd - ground power supply
 */
 

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <stdio.h>
#include <string.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#define TX_PIN A0 // Arduino transmit  Green WIRE  labeled TX on printer
#define RX_PIN A1 // Arduino receive   Blue WIRE   labeled RX on printer
#define SS_PIN 10
#define RST_PIN 9


const byte ROWS = 4;
const byte COLS = 4;

int incomingByte;
String data;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {3, 8, 7, 5};
byte colPins[COLS] = {4, 2, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

void setup()
{    
  mySerial.begin(9600); 
  printer.begin();
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void ask_for_UID()
{
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  //Show UID on serial monitor
  Serial.print("U");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println("");
}

void ask_for_key()
{
  char customKey = customKeypad.getKey();

  if (customKey){
    Serial.print("K");
    Serial.print(customKey);
    Serial.println("");
    }
}

void parseInput(){
  //data string is remembered and elements are being removed so can call function with null
  char *bedrag = strtok(data.c_str(), ",");
  char *locatie = strtok(NULL, ",");
  char *tijd = strtok(NULL, ",");
  char *datum = strtok(NULL, ",");
  char *rekeningNummer = strtok(NULL, ",");
  //send all the data too the printer
  printBon(bedrag, locatie, tijd, datum, rekeningNummer);
}

void printBon(char *bedrag, char *tijd, char *datum, char *rekeningNummer, char* locatie){
    //center the text
    //print all data from parseInput
    printer.justify('C');
    printer.println(bedrag);
    printer.println("");
    printer.println(tijd);
    printer.println("");
    printer.println(datum);
    printer.println("");
    printer.println(rekeningNummer);
    printer.println("");
    printer.println(locatie);
    printer.println("");
    printer.println("");
    //restore to default settings
    printer.setDefault();
}

void loop()
{
  ask_for_key();
  ask_for_UID();
  if(Serial.available() > 0){
    incomingByte = Serial.read();
    data += char(incomingByte);
    if(char(incomingByte) == '\0'){
   
      parseInput();
      //clear the string
      data = "";
      }
  }
}
