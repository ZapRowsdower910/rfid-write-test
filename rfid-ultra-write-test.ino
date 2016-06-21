/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com.
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BACKLIGHT_PIN (3)
#define LED_ADDR (0x27) // might need to be 0x3F, if 0x27 doesn't work
LiquidCrystal_I2C lcd(LED_ADDR, 2, 1, 0, 4, 5, 6, 7, BACKLIGHT_PIN, POSITIVE) ;

#define SS_PIN 10    //Arduino Uno
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.


void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  SPI.begin();                // Init SPI bus
  mfrc522.PCD_Init();        // Init MFRC522 card
  Serial.println(F("Write personal data on a MIFARE PICC "));

  lcd.begin(20, 4);               // initialize the lcd
  lcd.clear();
  lcd.home ();                   // go home
  lcd.print(" TeaCrane v1.1  ");
  delay(1000);

  lcd.clear();
  lcd.home ();                   // go home
  lcd.print("Waiting for card");

}

void loop() {

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())    return;

  lcd.clear();
  lcd.home ();                   // go home
  lcd.print("Reading Card..");

  lcd.setCursor(0, 1);
  Serial.print(F("Card UID:"));    //Dump UID
  lcd.print("UID: ");
  //        lcd.setCursor(0, 2);

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    lcd.print(mfrc522.uid.uidByte[i]);
  }

  lcd.setCursor(0, 2);

  Serial.print(F(" PICC type: "));   // Dump PICC type
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  byte status, len;

  Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial


  ///////////// Last Name ////////////////////

  lcd.print("Enter Name:");
  // Ask personal data: Family name
  Serial.println(F("Type name, ending with #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 30) ; // read family name from serial
  padString(buffer, len);

  block = 4;

  // Write block
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  block++;

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[4], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));



  ///////////// Tea Type //////////////////

  lcd.setCursor(0, 2);
  lcd.print("Select Tea Type:");

  Serial.println(F("Type the type of tea, ending with #"));
  len = Serial.readBytesUntil('#', (char *) buffer, 20) ; // read first name from serial
  padString(buffer, len);

  block++;

  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));

  block++;

  // Write block
  status = mfrc522.MIFARE_Write(block, &buffer[4], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else Serial.println(F("MIFARE_Write() success: "));


  ///////////// Time Override  /////////////////

  //  lcd.setCursor(0, 2);
  //  lcd.print("Enter Time Override");
  //
  //  Serial.println(F("Need a time override? You know the drill"));
  //  len=Serial.readBytesUntil('#', (char *) buffer, 20) ; // read first name from serial
  ////      for (byte i = len; i < 20; i++) buffer[i] = '\s';     // pad with spaces
  //
  //  block++;
  //
  //   status = mfrc522.MIFARE_Write(block, buffer, 16);
  //    if (status != MFRC522::STATUS_OK) {
  //        Serial.print(F("MIFARE_Write() failed: "));
  //        Serial.println(mfrc522.GetStatusCodeName(status));
  //              return;
  //    }
  //    else Serial.println(F("MIFARE_Write() success: "));


  Serial.println(" Done! ");
  Serial.println(" ");
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  lcd.clear();
  lcd.home ();                   // go home
  lcd.print("Write completed!");
  delay(1500);
  lcd.clear();
  lcd.home ();                   // go home
  lcd.print("Waiting for card");

}

void padString(byte *buffer, short len) {
  for (byte i = len; i < 20; i++) buffer[i] = 0x20;
}

