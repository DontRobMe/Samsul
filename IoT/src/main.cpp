#include <SPI.h>
#include <MFRC522.h>

#define SDA_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SDA_PIN, RST_PIN);

#define BUZZER_PIN 6

void setup() {
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  Serial.begin(115200);
  SPI.begin();

#ifdef __AVR__
  SPI.setClockDivider(SPI_CLOCK_DIV8);
#endif

  pinMode(BUZZER_PIN, OUTPUT);

  mfrc522.PCD_Init();
  delay(100);
  mfrc522.PCD_DumpVersionToSerial();

  Serial.println("Approchez une carte/tag...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;
  Serial.print("UID: ");

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX); Serial.print(" ");
  }
  Serial.println();

  tone(BUZZER_PIN, 600, 100);
  delay(120);

  mfrc522.PICC_HaltA();
}
