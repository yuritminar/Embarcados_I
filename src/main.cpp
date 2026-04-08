#include <Arduino.h>

/****** Bloco de configuração Wifi ******/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "secrets.h"
#include "MQUnifiedsensor.h" 


bool modoTeste = true; //Variável que define quando eu tenho o hardware em mãos ou não
const char* ssid = WIFI_SSID;  
const char* password = WIFI_PASS;  

AsyncWebServer server(80);

/****** Fim do bloco de coniguração Wifi *******/

/*configurações de relé*/
bool releLigado = false;


/**** Bloco de configuração do MQ-7 ****/
//Definições
#define placa "Esp-32"
#define Voltage_Resolution 3.3
#define pin 13 //Analog input 0 of your arduino
#define type "MQ-7" //MQ7
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm 
#define PWMPin 5 // Pin connected to mosfet

// Declaração do sensor
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
unsigned long oldTime = 0;



/**** Fim do bloco de configuração do MQ-7 ****/




/******* Bloco de configuração DHT22 ******/
#include <DHT.h>
#define DHT22_PIN 21 //pino que o DHT22 ta conectado
DHT dht22(DHT22_PIN, DHT22);
/****** Fim do bloco de configuração DHT22 ******/

/****** Bloco de configuração do MPL3115A2 ******/
#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
/****** Fim do bloco de configuração do MPL3115A2 ******/

#define led 2

/**** Definição de protótipos ****/
float le_temp();
float le_umid();
float le_alt();
float le_pres();
String processor(const String& var);

/*** Os protótipos abaixo existem para lidar com as variáveis de forma inteligente
 *  de modo que as mesmas possam rodar mesmo com o hardware faltando.
 *  Assim nada quebra de fato. ***/
float getTemp();
float getUmid();
float getPres();
float getCO();
float getAlt();

bool setVent();
bool setRele();
/**** Fim da definição de protótipos ****/

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(9600);
 
  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar SPIFFS!");
    Serial.println("Reinicie o ESP ou regrave o SPIFFS.");
    return;
  }
  
  dht22.begin();
 
  WiFi.begin(ssid, password);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.begin();

}

void loop() {

getTemp();
getUmid();


  
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

float getTemp() {
  if (modoTeste == true){
    return 25.0;
  } else {
   
    float temp = le_temp();
   if (isnan(temp)) {
    Serial.println("Failed to read Temperature from DHT22 sensor!");
    return 0;
    } else {
      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print("°C  ~  ");
      return temp;
    }

  }
}

float getUmid() {
  if (modoTeste == true){
    return 15;
  } else {

    float umid = le_umid();
    if (isnan(umid)) {
    Serial.println("Failed to read Umidity from DHT22 sensor!");
    return 0;
  } else {
    Serial.print("  |  ");

    Serial.print("Humidity: ");
    Serial.print(umid);
    Serial.print("%");
  }
  }
}
/**** Fim das Funções do DHT22 ****/

String processor(const String& var){
  if (var == "TEMP"){
    float temp = getTemp();
    String tempStr = String(temp, 2);
    return tempStr;
  } else if (var == "UMID"){
    float umid = getUmid();
    String umidStr = String(umid, 2);
    return umidStr;
  } else {
    return String("Sem valor");
  }
}