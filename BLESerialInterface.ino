/*

  Project: BLESerialInterface
  Created: 03.09.2020
  Author:  Primoz Flander

  v4: added error led blinking, refactoring code
  v1: demo

  Arduino board: Arduino Nano BLE
  
  ARDUIONO I/O   /     DESCRIPTION

*/

/*=======================================================================================
                                    Includes
  ========================================================================================*/

#include <ArduinoBLE.h>

/*=======================================================================================
                                    Definitions
  ========================================================================================*/
  
#define getInterval    50
#define listenInterval  500
#define ledR            22
#define ledG            23
#define ledB            24

/*=======================================================================================
                                User Configurarations
  ========================================================================================*/
  
unsigned long value;
unsigned long previousGetMillis = 0;
unsigned long previousListenMillis = 0;
int customRange;
int customValue;

/*=======================================================================================
                                   Setup function
  ========================================================================================*/
  
void setup() {

  Serial.begin(9600);
  delay(2000);
  //while (!Serial);
  initIO();
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    error();
  }
  BLE.scan();
  digitalWrite(ledB,LOW);
}

/*=======================================================================================
                                            Loop
  ========================================================================================*/
  
void loop() {

  BLEDevice peripheral = BLE.available();

  if (peripheral) { 
    if (peripheral.localName() == "SensoGrip") {
      BLE.stopScan();
      digitalWrite(ledB,HIGH);
      BSInterface(peripheral);
      BLE.scan();
      digitalWrite(ledB,LOW);
    }
  }
}

void BSInterface(BLEDevice peripheral) {
  if (peripheral.connect()) {
    //Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }
  if (peripheral.discoverService("1111")) {
    //Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();
    error();
  }

  BLECharacteristic tipChar = peripheral.characteristic("2001");
  BLECharacteristic fingerChar = peripheral.characteristic("2002");
  BLECharacteristic accXChar = peripheral.characteristic("2003");
  BLECharacteristic accYChar = peripheral.characteristic("2004");
  BLECharacteristic accZChar = peripheral.characteristic("2005");
  BLECharacteristic timeChar = peripheral.characteristic("2006");
  BLECharacteristic feedbackChar = peripheral.characteristic("2007");
  BLECharacteristic refChar = peripheral.characteristic("2008");
  BLECharacteristic rangeChar = peripheral.characteristic("2009");

  while (peripheral.connected()) {
    
    digitalWrite(ledG,LOW);
    tipChar.readValue(value);
    Serial.print(value);
    Serial.print(" ");
    timeChar.readValue(value);
    Serial.print(value);
    Serial.println();
    
    if (Serial.available() > 0) {
      String receivedData = Serial.readString();
      if (receivedData == "p1") {
        value = 1200;
        rangeChar.writeValue(value);
      }
      else if (receivedData  == "p2") {
        value = 800;
        rangeChar.writeValue(value);
      }
      else if (receivedData  == "p3") {
        value = 400;
        rangeChar.writeValue(value);
      }
      else if (receivedData  == "fbpos")  {
        value = true;
        feedbackChar.writeValue(value);
      }
      else if (receivedData  == "fbneg")  {
        value = false;
        feedbackChar.writeValue(value);
      }
      else if (receivedData.startsWith("p"))  {
        parseString(receivedData); 
        value = customValue;
        refChar.writeValue(value);
        value = customRange;
        rangeChar.writeValue(value);
      }
    }
  }
  //Serial.println("Disconnected!");
  digitalWrite(ledG,HIGH);
}

/*=======================================================================================
                                         Functions
  ========================================================================================*/
  
void parseString(String str)  {

  char string[15];
  str.toCharArray(string, 15);
  char *ptr;
  ptr = strchr(string, ',');
  if (ptr != NULL)
  {
    *ptr = '\0';
    customRange = atoi(&string[1]);
    ptr++;
    customValue = atoi(ptr);
  }
}

void error(void)  {

  while (1) {
    digitalWrite(ledR,HIGH);
    delay(500);
    digitalWrite(ledR,LOW);
    delay(500);
  }  
}

void initIO() {
  
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  digitalWrite(ledR,HIGH);
  digitalWrite(ledG,HIGH);
  digitalWrite(ledB,HIGH); 
}
