 /* 
 *  Script para correr un solo lector de RFID.
 *  ESP36 WROOM
 */

#include <SPI.h>      // incluye libreria bus SPI
#include <MFRC522.h>      // incluye libreria especifica para MFRC522
#include "WiFi.h"
#include <PubSubClient.h>


WiFiClient esp32Client;
PubSubClient mqttClient(esp32Client);


const char* ssid     = "AXTEL XTREMO-AECE";
const char* password = "0359AECE";

char *server = "softechiot.com";
int port = 1883;


#define WIFI_NETWORK "AXTEL XTREMO-AECE"
#define WIFI_PASSWORD "0359AECE"
#define WIFI_TIMEOUT_MS 20000

void wifiInit() {
    Serial.print("Conectándose a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
        delay(500);  
    }
    Serial.println("");
    Serial.println("Conectado a WiFi");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
  }

void connectToWifi()
{
  
 Serial.print("Connecting WIFI ");
 WiFi.mode(WIFI_STA);
 WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

 unsigned long startAttemptTime = millis();

 while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
  
    Serial.print(".");
    delay(100);
  }

 if(WiFi.status() != WL_CONNECTED)
 {
    Serial.print( "Failed");
  }
  else{
    
      Serial.print("Connected!");
      Serial.println(WiFi.localIP());
     }
 
}



#define RST_PIN 22      // constante para referenciar pin de reset
#define SS_PIN  5      // constante para referenciar pin de slave select
#define SS_PIN_2  17      // constante para referenciar pin de slave select


MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto mfrc522 enviando pines de slave select y reset

int var = 0;
int ledval = 0;
int fotoval = 0;
char datos[40];
String resultS = "";

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  char payload_string[length + 1];
  
  int resultI;

  memcpy(payload_string, payload, length);
  payload_string[length] = '\0';
  resultI = atoi(payload_string);
  
  var = resultI;

  resultS = "";
  
  for (int i=0;i<length;i++) {
    resultS= resultS + (char)payload[i];
  }
  Serial.println();
}




void reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Intentando conectarse MQTT...");

    if (mqttClient.connect("arduinoClient", "test", "chicharito17A5")) {
      Serial.println("Conectado");

      mqttClient.subscribe("relay1");
      
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" intentar de nuevo en 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  
  
  Serial.begin(115200);     // inicializa comunicacion por monitor serie a 9600 bps
  //connectToWifi();
  delay(10);
  wifiInit();
  SPI.begin();        // inicializa bus SPI
  mfrc522.PCD_Init();     // inicializa modulo lector
  mqttClient.setServer(server, port);
  mqttClient.setCallback(callback);
  
}

void loop() {

  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  
  if ( ! mfrc522.PICC_IsNewCardPresent()) // si no hay una tarjeta presente
    return;         // retorna al loop esperando por una tarjeta
  
  if ( ! mfrc522.PICC_ReadCardSerial())   // si no puede obtener datos de la tarjeta
    return;         // retorna al loop esperando por otra tarjeta
    
  Serial.print("UID:");       // muestra texto UID:
  String code;
  for (byte i = 0; i < mfrc522.uid.size; i++) { // bucle recorre de a un byte por vez el UID
    if (mfrc522.uid.uidByte[i] < 0x10){   // si el byte leido es menor a 0x10
      Serial.print(" 0");     // imprime espacio en blanco y numero cero
      }
      else{         // sino
      Serial.print(" ");      // imprime un espacio en blanco
      }

      if(i == 0)
      {
        code = String(mfrc522.uid.uidByte[i], HEX);
      }
      else{
          code = code + "" + String(mfrc522.uid.uidByte[i], HEX);
        }
      
  
  } 
  char const *code2 = code.c_str();
  Serial.print(code2);  // imprime el byte del UID leido en hexadecimal  
  mqttClient.publish("relay1",code2);     
  Serial.println();       // nueva linea
  mfrc522.PICC_HaltA();                   // detiene comunicacion con tarjeta
  delay(10);
}
