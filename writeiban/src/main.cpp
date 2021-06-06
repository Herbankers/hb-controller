#include <Arduino.h>

#include <MFRC522.h>
#include <SPI.h>

#define RST_PIN		9
#define SS_PIN		10

#define IBAN_LENGTH	18

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

#define BLOCK_SIZE	16

#define SECTOR		0	/* First sector */
#define DATA1_BLOCK	1	/* First data block (not using 0 bcs it's the Manufacturer Block) */
#define DATA2_BLOCK	2	/* Second data block */
#define TRAILER_BLOCK	3	/* Sector 0 trailer block */

byte iban[BLOCK_SIZE * 2];	/* IBAN buffer */

void loop()
{
	byte buf_size = BLOCK_SIZE + 2;
	byte buf[buf_size * 2];

	/* continue if no card is present */
	if (!rfid.PICC_IsNewCardPresent())
		return;
	if (!rfid.PICC_ReadCardSerial())
		return;

	/* check if this is a MIFARE classic */
	MFRC522::PICC_Type type = rfid.PICC_GetType(rfid.uid.sak);

	Serial.print("Checking compatibility... ");

	if (type != MFRC522::PICC_TYPE_MIFARE_1K) {
		Serial.println("ERROR");
		Serial.println(" Card is not supported");
		goto err;
	}

	Serial.println("OK");

	/* authenticate key A (but apparently we don't need this to write to block 0 and 1 */
	Serial.println("Authenticating... ");

	/* if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, TRAILER_BLOCK, &key, &rfid.uid) != 0) {
		Serial.println("ERROR");
		Serial.println(" Key A authentication failed");
		goto err;
	} */

	/* authenticate key B */
	if (rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, TRAILER_BLOCK, &key, &rfid.uid) != 0) {
		Serial.println("ERROR");
		Serial.println(" Key B authentication failed");
		goto err;
	}

	Serial.println("OK");

	/* write the IBAN to the card */
	Serial.print("Writing... ");

	if (rfid.MIFARE_Write(DATA1_BLOCK, iban, 16) != 0) {
		Serial.println("ERROR");
		Serial.println(" Writing to block 1 failed");
		goto err;
	}

	if (rfid.MIFARE_Write(DATA2_BLOCK, iban + 16, 16) != 0) {
		Serial.println("ERROR");
		Serial.println(" Writing to block 2 failed");
		goto err;
	}

	Serial.println("OK");

	/* verify the written data */
	Serial.print("Verifying... ");

	if (rfid.MIFARE_Read(DATA1_BLOCK, buf, &buf_size) != 0) {
		Serial.println("ERROR");
		Serial.println(" Reading from block 1 failed");
		goto err;
	}

	/* yes we're overwriting the CRC here, bcs we won't be checking it. Kinda lazy but what evvs */
	if (rfid.MIFARE_Read(DATA2_BLOCK, buf + BLOCK_SIZE, &buf_size) != 0) {
		Serial.println("ERROR");
		Serial.println(" Reading from block 2 failed");
		goto err;
	}

	for (int i = 0; i < IBAN_LENGTH; i++) {
		if (buf[i] != iban[i]) {
			Serial.println("ERROR");
			Serial.println(" Verification failed");
			goto err;
		}
	}

	Serial.println("OK");

	/* finalize */
	Serial.println();
	Serial.println("Done!");

	rfid.PICC_HaltA();
	rfid.PCD_StopCrypto1();

err:
	delay(2000);
	return;
}

void setup()
{
	/* wait for serial connection to be opened */
	Serial.begin(9600);
	while (!Serial);

	/* initialize the RFID reader */
	SPI.begin();
	rfid.PCD_Init();

	/* use the default key because I guess that's what everyone is using */
	for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
		key.keyByte[i] = 0xFF;

	Serial.println("Please enter the IBAN you wish to write to a MIFARE Classic 1K card.");
	Serial.print("IBAN: ");
	memset(iban, 0, sizeof(iban));
	for (int i = 0; i < IBAN_LENGTH; i++) {
		while (!Serial.available());

		iban[i] = Serial.read();
		Serial.print((char) iban[i]);
	}
	Serial.println();

	Serial.println("Please present a card to write to...");
}
