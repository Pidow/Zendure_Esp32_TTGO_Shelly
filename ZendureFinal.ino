#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>

const char* ssid = "YOUR SSID";
const char* password = "PASSWORD";
const String request = "http://IPADRESSOFSHELLY/rpc/Shelly.GetStatus" ;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fr.pool.ntp.org", 3600, 60000);

float power = 0; // chaine a relever
String POWER = "w"; // unité w u Kw
String MESURE = "Prod"; // production ou conso
int ecran = 1; //ecran allume ou eteint
int buttonState1 = 0; // appui bouton
int buttonState2 = 0; // appui bouton
int hourstart = 6;// heure debut affichage ecran
int hourstop = 20;// heure fin affichage ecran
int currentHour;


JsonDocument doc;
#define BUTTON1PIN 35
#define BUTTON2PIN 0

TFT_eSPI tft = TFT_eSPI();

const String url = "https://app.zendure.tech/eu";
const String ZendureDeveloperUrl = "http://app.zendure.tech/eu/developer/api/apply";
const String serialNumber = "SERIALNUMBER";
const String appUsername = "YOURZENDUREMAIL";
String httpRequestData = " ";

const char *mqtt_broker = "mqtADRESS"; 
const char *topic = "TOPICADRESS/state/#"; 
const char *mqtt_username = "MQTTUSERNAME"; 
const char *mqtt_password = "MQTTPASSWORD"; 
const int mqtt_port = PORT;
int batterie = 0;
int Updatbat = 0;

WiFiClient espClient; 
PubSubClient client(espClient); 


      
void setup() {
  WiFi.begin(ssid, password);
  tft.init();
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(25, 60);
  tft.print("WIFI CONNECTION");
  tft.println(" "); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    tft.print("*");
  }
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(80, 60);
  tft.print("WIFI OK");
  delay(1500);
  pinMode(BUTTON1PIN, INPUT);
  pinMode(BUTTON2PIN, INPUT);
  timeClient.begin();

  //Activer pour avoir les codes mqtt
  //zendure();
  mqtt();
}

void loop() {
  
    timeClient.update();
     currentHour = timeClient.getHours();
    if ((currentHour >= hourstop) && (ecran == 1)) {
      screenoff ();
      }
    if ((currentHour >= hourstart) && (currentHour < hourstop) && (ecran == 0)) {
      screenon ();
      }
    buttonState1 = digitalRead(BUTTON1PIN);
    buttonState2 = digitalRead(BUTTON2PIN);
    if ((buttonState1 == LOW) || (buttonState2 == LOW)) {
    screenon ();
      }
     if (ecran == 1) {
     shelly();
     calc();
     if ((batterie != 0) || (Updatbat!=600)) {
      delay (10000);
     }
     
     while ((batterie == 0) || (Updatbat==600)){
      batterie = 0;
      Updatbat = 0;
       client.loop();//recuperation valeur batterie
      }
      Updatbat = Updatbat++;
     heure ();
     }
}

void heure() {
    timeClient.update();
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(5);
    tft.setCursor(0,0);
    tft.print(""+timeClient.getFormattedTime()+"");
    delay (1000);
    tft.fillScreen(TFT_BLACK);
    timeClient.update();
    tft.print(""+timeClient.getFormattedTime()+"");
    delay (1000);
    tft.fillScreen(TFT_BLACK);
    timeClient.update();
    tft.print(""+timeClient.getFormattedTime()+"");
    delay (1000);
      }
void screenoff() {
      pinMode(4,OUTPUT);
      digitalWrite(4,LOW);
      tft.writecommand(ST7789_DISPOFF);
      tft.writecommand(ST7789_SLPIN);
      ecran = 0;
      }
void screenon() {
      pinMode(4,OUTPUT);
      digitalWrite(4,HIGH);
      tft.writecommand(ST7789_DISPON);
      tft.writecommand(ST7789_SLPOUT);
      ecran = 1;
      }

void shelly() { 
  if (WiFi.status() == WL_CONNECTED) {  
    WiFiClient client;
    HTTPClient http;
    http.useHTTP10(true);
    http.begin(client, request);
    int httpCode = http.GET();
    if (httpCode == 200 ) {
      String reponse = http.getString();
      deserializeJson(doc, reponse); 
      power = doc["em:0"]["c_act_power"];
    } else {
      tft.print("Error - HTTP code : ");
      tft.print(httpCode);
      http.end();
    }
    } else  {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(80, 60);
      tft.print("WIFI DOWN");  
    }
}

void calc() { 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(0xFFFF);  
  tft.setCursor(0, 10);
  tft.setTextSize(3);
  tft.print(String("BATTERIE: ")+batterie+"%"); 
   if ((power > 0) && (power < 1000))
      {
        tft.setTextColor(0xF384);
        POWER = ("w");
        MESURE = ("CONSOMMATION");
      }
   if (power > 999)
       {
        tft.setTextColor(0xF384);
        power /= 1000;
        POWER = ("Kw");
        MESURE = ("CONSOMMATION");
       }
  if ((power < 0) && (power > -1000))
      {
        tft.setTextColor(0x068A);
        power *= -1;
        POWER = ("w");
        MESURE = (" PRODUCTION");
      }
  if (power < -999)
      {
        tft.setTextColor(0x068A);
        power /= -1000;
        POWER = ("Kw");
        MESURE = (" PRODUCTION");
    }
    
  tft.setCursor(10, 45);
  tft.setTextSize(3);
  tft.print(MESURE);
  tft.setCursor(30, 80);
  tft.setTextSize(7);
  if (POWER == "w")
      {
        tft.print((power),0);
      } else {
        tft.print((power),1);
      }
  tft.print(POWER);
}

void zendure() { // connection au zendure pour recuperer les codes de connection MQTT
    HTTPClient http;
    http.begin(ZendureDeveloperUrl);
    http.addHeader("Content-Type", "application/json");//CONTENT JSON
    JsonDocument doc;
    doc["snNumber"] = serialNumber;
    doc["account"] = appUsername;
    serializeJson(doc, httpRequestData);
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode == 200) {
      String reponse = http.getString();
      deserializeJson(doc, reponse); 
      String mqtta_login  = doc["data"]["appKey"];
      String mqtta_pass = doc["data"]["secret"];
      String mqtta_url = doc["data"]["mqttUrl"];
      int mqtta_port = doc["data"]["port"];
      String topica = ""+mqtta_login+"/#"; 
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0);
      tft.setTextSize(2);
      tft.print("*  ZENDURE API: OK *");
      tft.print("Compte MQTT:");
      tft.println (mqtta_login);
      tft.print("Pass:");
      tft.println (mqtta_pass);
      tft.print("URL:");
      tft.println (mqtta_url);
      tft.print("Port:");
      tft.println (mqtta_port);
      http.end();
      delay (50000);
    } else {
        tft.print("*  ZENDURE API: NOK *");
        http.end();
        delay (50000);
    }
}
void mqtt() {  
   tft.fillScreen(TFT_BLACK);
   tft.setCursor(50, 60);
   client.setServer(mqtt_broker, mqtt_port); 
   client.setCallback(callback); 
   while (!client.connected()) { 
      String client_id = "esp32-client-"; 
      client_id += String(WiFi.macAddress()); 
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) { 
          if (batterie == 0) {
            tft.println("ATTENTE MQTT"); 
          }
        client.subscribe(topic); 
      } else { 
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.print("Erreur MQTT: "); 
        tft.print(client.state());
        delay(50000);
      }
    }

}

void callback(char* topic, byte* payload, unsigned int length)  {        
  char messageBuffer[200];
  memcpy(messageBuffer, payload, length);
  messageBuffer[length] = '\0';
  parseMessage(messageBuffer);
}
 
 void parseMessage(char* json)  {
    StaticJsonDocument<300> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error)
    {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.print(F("Erreur JSON: "));
        tft.println(error.f_str());
        delay(50000);
        return;
    }
    batterie = doc["electricLevel"];
}
