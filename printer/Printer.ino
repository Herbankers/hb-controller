#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"
#define TX_PIN 6 // Arduino transmit  YELLOW WIRE  labeled TX on printer
#define RX_PIN 5 // Arduino receive   GREEN WIRE   labeled RX on printer

String currentTime = "15.45";
String datum = "27/5/2021";
String iban = "*******5567";
String locatie = "nederland";

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor

void setup() {
  // put your setup code here, to run once:
  pinMode(7, OUTPUT); 
  digitalWrite(7, LOW);
  mySerial.begin(9600); 
  printer.begin();  
}




void printBon(String tijd, String datum, String iban, String locatie){
    //center the text
    printer.justify('C');
    printer.println(tijd);
    printer.println("");
    printer.println(datum);
    printer.println("");
    printer.println(iban);
    printer.println("");
    printer.println(locatie);
    //restore to default settings
    printer.setDefault();
  }
void loop() {
  // put your main code here, to run repeatedly:
  printBon(currentTime, datum, iban, locatie);
    
}
