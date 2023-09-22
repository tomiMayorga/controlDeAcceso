/*
 * --------------------------------------------------------------------------------------------------------------------
 * LIBRERIA ARDUINO MFRC522 by GithubCommunity.
 * --------------------------------------------------------------------------------------------------------------------
 * Libreria: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define CANTIDAD_LLAVES 20


MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte tarjetaLeida[4];
byte tarjetasHabilitadas[CANTIDAD_LLAVES][4] = { { 154, 175, 242, 129 } };
bool banderaTarjetaHabilitada;
//sirve para dar de alta y dar de baja
int cantLlavesHabilitadas;

void setup() {
  Serial.begin(9600);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  Serial.println(tarjetasHabilitadas[0][0]);
  Serial.println(tarjetasHabilitadas[0][1]);
  Serial.println(tarjetasHabilitadas[0][2]);
  Serial.println(tarjetasHabilitadas[0][3]);
}

void loop() {

  // Look for new cards
  if (rfid.PICC_IsNewCardPresent()) {
    
    // Verify if the NUID has been readed
    if (!rfid.PICC_ReadCardSerial()) {
      Serial.print("hola2");
      return;
    }
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    Serial.println(rfid.PICC_GetTypeName(piccType));

    // Check is the PICC of Classic MIFARE type
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("Your tag is not of type MIFARE Classic."));
      return;
    }

    if (rfid.uid.uidByte[0] != tarjetaLeida[0] || rfid.uid.uidByte[1] != tarjetaLeida[1] || rfid.uid.uidByte[2] != tarjetaLeida[2] || rfid.uid.uidByte[3] != tarjetaLeida[3]) {
      Serial.println(F("Se detecto una nueva tarjeta/tag."));

      // Store NUID into nuidPICC array //Preguntarle a Lean si se guarda aca la tarjeta.
      for (byte i = 0; i < 4; i++) {
        tarjetaLeida[i] = rfid.uid.uidByte[i];
      }

      Serial.println(F("El NUID tag es:"));
      Serial.print(F("En hex: "));
      printHex(rfid.uid.uidByte, rfid.uid.size);
      Serial.println();
      Serial.print(F("En dec: "));
      printDec(rfid.uid.uidByte, rfid.uid.size);
      Serial.println();
    } else Serial.println(F("Tarjeta/Tag leida previamente."));


    //Guardar llave nueva
    /*
  for(int fila = 0; fila < 2 ; fila++){
    for(int columna = 0; columna < 2; columna++){
      matriz[fila][columna] = tarjetaHabilitada[];
    }
  }

  */

    //Pregunto si la llave leida esta habilitada
    for (int fila = 0; fila < CANTIDAD_LLAVES; fila++) {
      banderaTarjetaHabilitada = true;
      for (byte i = 0; i < 4; i++) {

        if (tarjetaLeida[i] != tarjetasHabilitadas[fila][i]) {
          banderaTarjetaHabilitada = false;
          break;
        }
      }

      if (banderaTarjetaHabilitada == true) {
        break;
      }
    }

    if (banderaTarjetaHabilitada == true) {
      Serial.print("ACCESO PERMITIDO");
    } else {
      Serial.print("ACCESO DENEGADO");
    }

    // Halt PICC
    rfid.PICC_HaltA();

    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();
  }

  if (Serial.available() > 0) {
    char datoRecibido = Serial.read();

    if (datoRecibido == 'c') {
      Serial.println("Entre a configuraciòn: \n\n'a': Dar de alta.\n'b': Dar de baja.");
    }
    if (datoRecibido == 'a') {
      //Si doy de alta tengo que agregar a mi lista el nuevo tag o tarjeta leida
      Serial.println("Quiere dar de alta");
    }
    if (datoRecibido == 'b') {
      //Si doy de baja tengo que sacar de mi lista el tag o tarjeta y subir todo para arriba, cosa de agregar siempre al final.
      Serial.println("Quiere dar de baja");
    }
  }
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}