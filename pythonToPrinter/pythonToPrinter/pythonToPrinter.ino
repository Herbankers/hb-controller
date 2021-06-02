#include <stdio.h>
#include <string.h>
#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#define TX_PIN 6 // Arduino transmit  Green WIRE  labeled TX on printer
#define RX_PIN 5 // Arduino receive   Blue WIRE   labeled RX on printer

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

int incomingByte;
String data;

void setup() {
  mySerial.begin(9600); 
  printer.begin();
  Serial.begin(9600);
    

}

void parseInput(){
  char *bedrag = strtok(data.c_str(), ",");
  char *locatie = strtok(NULL, ",");
  char *tijd = strtok(NULL, ",");
  char *datum = strtok(NULL, ",");
  char *rekeningNummer = strtok(NULL, ",");
  printBon(bedrag, locatie, tijd, datum, rekeningNummer);
}

void printBon(char *bedrag, char *tijd, char *datum, char *rekeningNummer, char* locatie){
    //center the text
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
//Serial.println(bedrag);
//Serial.println(tijd);
//Serial.println(datum);
//Serial.println(locatie);
//Serial.println(rekeningNummer);
}


void loop() {
  if(Serial.available() <= 0){
    return;
  }

  incomingByte = Serial.read();
  
  data += char(incomingByte);

  /*as soon as the termininate signal has been received 
  we need to clear the string for new input as the received data has been given to
  the parseInput function.
  */
  if(char(incomingByte) == '\0'){
    parseInput();
    //clear the string
    data = "";
  }    
}
