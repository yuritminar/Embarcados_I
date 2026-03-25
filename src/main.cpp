#include <Arduino.h>

/****** Bloco de configuração Wifi ******/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "secrets.h"


const char* ssid = WIFI_SSID;  
const char* password = WIFI_PASS;  

AsyncWebServer server(80);

/****** Fim do bloco de coniguração Wifi *******/

/******* Bloco de configuração DHT22 ******/

#include <DHT.h>
#define DHT22_PIN 21 //pino que o DHT22 ta conectado
DHT dht22(DHT22_PIN, DHT22);


/****** Fim do bloco de configuração DHT22 ******/

#define led 2
/**** Definição de protótipos ****/
float le_temp();
float le_umid();
/**** Fim da definição de protótipos ****/

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(9600);
  dht22.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  float temp = le_temp();
  float umid = le_umid();


  if ( isnan(temp) ||isnan(umid)) {
    Serial.println("Failed to read from DHT22 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(umid);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C  ~  ");

  }

  String tempStr = String(temp, 2);
  String umidStr = String(umid, 2);
}



// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
/**** Funções do DHT22 ****/
  float le_temp() 
{
  float temp = dht22.readTemperature();
  return float(temp);
}
  
float le_umid() 
{
  float umid = dht22.readHumidity();
  return float(umid);
}


/**** Fim das Funções do DHT22 ****/