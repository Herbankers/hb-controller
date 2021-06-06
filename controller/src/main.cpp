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
#define TX_PIN	A0
#define RX_PIN	A1

SoftwareSerial printer(RX_PIN, TX_PIN);

/* rfid */
#define RST_PIN	9
#define SS_PIN	10

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

#define SECTOR		0	/* First sector */
#define DATA1_BLOCK	1	/* First data block (not using 0 bcs it's the Manufacturer Block) */
#define DATA2_BLOCK	2	/* Second data block */

#define BLOCK_SIZE	16
#define IBAN_LENGTH	18

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

/* read the pressed key from the keypad */
void readKey()
{
	char key = customKeypad.getKey();

	/* write the key on the serial line */
	if (key)
		Serial.println("K" + String(key));
}

/* read the UID and IBAN from RFID card */
void readCard()
{
	/* continue if no card is present */
	if (!rfid.PICC_IsNewCardPresent())
		return;
	if (!rfid.PICC_ReadCardSerial())
		return;

	/* authenticate key B */
	if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, SECTOR + 3, &key, &rfid.uid) != 0)
		return;

	byte buf_size = BLOCK_SIZE + 2;
	byte buf[buf_size * 2];

	/* yes we're overwriting the CRC here, bcs we won't be checking it. Kinda lazy but what evvs */
	if (rfid.MIFARE_Read(DATA1_BLOCK, buf, &buf_size) != 0 ||
			rfid.MIFARE_Read(DATA2_BLOCK, buf + BLOCK_SIZE, &buf_size) != 0)
		return;

	/* print the UID */
	Serial.print("U");
	for (int i = 0; i < rfid.uid.size; i++) {
		Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
		Serial.print(rfid.uid.uidByte[i], HEX);
	}
	Serial.println();

	/* print the IBAN */
	Serial.print("I");
	for (int i = 0; i < IBAN_LENGTH; i++)
		Serial.print((char) buf[i]);
	Serial.println();

	/* finalize */
	rfid.PICC_HaltA();
	rfid.PCD_StopCrypto1();
}

/* check the serial USB line for commands */
void readSerial()
{
	if (Serial.available() <= 0)
		return;

	/*
	 * allow the '[' character to switch to forwarding USB serial data directly
	 * to the thermal printer's serial line until ']' in encountered
	 */
	uint8_t c = (uint8_t) Serial.read();
	if (c != '[')
		return;

	/* printer serial forwarding */
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

void loop()
{
	readKey();
	readCard();
	readSerial();
}

void setup()
{
	/* wait for serial connection to be opened */
	Serial.begin(9600);
	while (!Serial);

	/* initialize all peripherals */
	printer.begin(9600);
	SPI.begin();
	rfid.PCD_Init();

	/* use the default key because I guess that's what everyone is using */
	for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
		key.keyByte[i] = 0xFF;
}
