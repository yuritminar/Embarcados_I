#include <Arduino.h>

/****** Bloco de configuração Wifi ******/
#include <WiFi.h>
#include <WebServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "secrets.h"
#include "MQUnifiedsensor.h" 

const char* ssid = WIFI_SSID;  
const char* password = WIFI_PASS;  

AsyncWebServer server(80);

/****** Fim do bloco de coniguração Wifi *******/

bool modoTeste = true; //Variável que define quando eu tenho o hardware em mãos ou não

/*configurações de relé*/
bool releLigado = false;


/**** Bloco de configuração do MQ-7 ****/
//Definições
#define placa "Esp-32"
#define Voltage_Resolution 3.3
#define MQ_Pin 13 //Analog input 0 of your arduino
#define MQ_type "MQ-7" //MQ7
#define ADC_Bit_Resolution 12 // For arduino UNO/MEGA/NANO
#define RatioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm 
#define PWMPin 5 // Pin connected to mosfet

// Declaração do sensor

/**** Bloco de configuração do MQ-7 ****/
MQUnifiedsensor MQ7(placa, Voltage_Resolution, ADC_Bit_Resolution, MQ_Pin, MQ_type);
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
float le_CO();
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

void setup() {
  
  Serial.begin(9600);
 
  if (!SPIFFS.begin()) {
    Serial.println("Erro ao montar SPIFFS!");
    Serial.println("Reinicie o ESP ou regrave o SPIFFS.");
    return;
  }
  
  if(modoTeste == false){
    dht22.begin();

    Wire.begin();

    if (!baro.begin()) {
      Serial.println("Erro ao inicializar MPL3115A2!");
    }

    MQ7.setRegressionMethod(1); // método padrão
    MQ7.init();
    MQ7.setR0(10); // valor inicial (ideal calibrar depois)
} else {  
    Serial.println("Modo de teste ativado. Valores fixos serão usados.");
  }

  WiFi.begin(ssid, password);
  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    tentativas++;
  }
  Serial.println("");
  Serial.println("WiFi conectado!");
  Serial.println(WiFi.localIP());

  server.serveStatic("/", SPIFFS, "/");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
  });
  server.begin();

}

void loop() {

Serial.print(getTemp());
delay(2000);
//getTemp();
//getUmid();


  
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
    return umid;
  }
  }
}
/**** Fim das Funções do DHT22 ****/

/**** Funções do MQ-7 ****/
float le_CO() {
  MQ7.update(); //Atualiza os valores do sensor
  float CO = MQ7.readSensor(); //Lê o valor do sensor
  return CO;
}

float getCO() {
  if (modoTeste == true){
    return 100;
  } else {
    float CO = le_CO();
    Serial.print("  |  ");
    Serial.print("CO: ");
    Serial.print(CO);
    Serial.println(" ppm");
    return CO;
  }
}
/**** Fim das funções do MQ-7 ****/

/**** Funções do MPL3115A2 ****/
float le_alt() {
  float alt = baro.getAltitude();
  return alt;
}

float le_pres() {
  float pres = baro.getPressure();
  return pres;
}

float getAlt() {
  if (modoTeste == true){
    return 500;
  } else {
    float alt = le_alt();
    Serial.print("  |  ");
    Serial.print("Altitude: ");
    Serial.print(alt);
    Serial.println(" m");
    return alt;
  }
}

float getPres() {
  if (modoTeste == true){
    return 1000;
  } else {
    float pres = le_pres();
    Serial.print("  |  ");
    Serial.print("Pressure: ");
    Serial.print(pres);
    Serial.println(" hPa");
    return pres;
  }
}
/**** Fim das funções do MPL3115A2 ****/


// else if (var == "VENT_STATUS") {
//    if (setVent()) {
//      return String("Ventoinha ligada/desligada com sucesso!");
//    } else {
//      return String("Falha ao ligar/desligar a ventoinha.");
//    }
//  } else if (var == "RELE_STATUS") {
//    if (setRele()) {
//      return String("Relé ligado/desligado com sucesso!");
//    } else {
//      return String("Falha ao ligar/desligar o relé.");
//    }
//





/**** funções da ventoinha e do relé ****/
bool setVent() {
  // Lógica para ligar ou desligar a ventoinha
  // Exemplo: digitalWrite(PWMPin, HIGH) para ligar, digitalWrite(PWMPin, LOW) para desligar
  return true; // Retorna true se a operação foi bem-sucedida
}

bool setRele() {
  // Lógica para ligar ou desligar o relé
  // Exemplo: digitalWrite(relePin, HIGH) para ligar, digitalWrite(relePin, LOW) para desligar
  return true; // Retorna true se a operação foi bem-sucedida
}
/**** Fim das funções da ventoinha e do relé ****/

/**** função processor ****/
String processor(const String& var){
  if (var == "TEMP"){
    float temp = getTemp();
    String tempStr = String(temp, 2);
    return tempStr;
  //} else if (var == "UMID"){
  //  float umid = getUmid();
  //  String umidStr = String(umid, 2);
  //  return umidStr;
  //} else if (var == "CO") {
  //  float CO = getCO();
  //  String COStr = String(CO, 2);
  //  return COStr;
  //} else if (var == "ALT") {
  //  float alt = getAlt();
  //  String altStr = String(alt, 2);
  //  return altStr;
  //} else if (var == "PRES") {
  //  float pres = getPres();
  //  String presStr = String(pres, 2);
  //  return presStr;
  } else {
    return String();
  }
}
/**** Fim da função processor ****/