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
#include "SoftwareSerial.h"

/* printer */
#define TX_PIN A0
#define RX_PIN A1

SoftwareSerial printer(RX_PIN, TX_PIN);

/* rfid */
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

/* keypad */
const byte ROWS = 4;
const byte COLS = 4;

char keymap[ROWS][COLS] = {
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'},
	{'*', '0', '#'}
};
byte rowPins[ROWS] = { 3, 8, 7, 5 };
byte colPins[COLS] = { 4, 2, 6 };

Keypad customKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);

void setup()
{
	Serial.begin(9600);

	/* initialize all peripherals */
	printer.begin(9600);
	SPI.begin();
	mfrc522.PCD_Init();
}

void ask_for_UID()
{
	if (!mfrc522.PICC_IsNewCardPresent())
		return;

	if (!mfrc522.PICC_ReadCardSerial())
		return;

	String content = "";

	/* write the UID on the serial line */
	Serial.print("U");
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
	char key = customKeypad.getKey();

	/* write the key on the serial line */
	if (key)
		Serial.println("K" + String(key));
}

void loop()
{
	uint8_t c;

	ask_for_key();
	ask_for_UID();

	if (Serial.available() <= 0)
		return;

	/*
	 * allow the '[' character to switch to forwarding USB serial data directly
	 * to the thermal printer's serial line until ']' in encountered
	 */
	c = (uint8_t) Serial.read();
	if (c != '[')
		return;

	for (;;) {
		if (Serial.available() <= 0)
			continue;

		c = (uint8_t) Serial.read();
		if (c == ']')
			break;

		printer.write(c);

		if (printer.available() > 0)
			Serial.write(printer.read());
	}
}
