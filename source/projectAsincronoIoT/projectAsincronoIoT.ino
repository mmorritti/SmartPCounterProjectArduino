/*
 * SMART P. COUNTER 2.0
 * Il progetto si pone come obiettivo quello di contare le persone in ingresso e in uscita
 * da un locale.
 * Per fare ciò ho utilizzato un sensore ultrasonico, installato su una porta, ogni volta che una persona passa davanti
 * al sensore viene inviato un 1 via mqtt a node-red che a sua volta aggiornerà un contatore
 * Inoltre node-red invia a sua volta il totale del contatore, sempre via mqtt, e il valore 
 * sarà visualizzato su uno schermo oled.
 * 
 * N.B.
 * Per creare questo progetto ho utilizzato le seguenti librerie:
 * 1. Grove Ultrasonic Ranger per la gestione del sensore ultrasonico
 * 2. Adafruit SSD1306 per la gestione dello schermo OLED
 * 3. Adafruit GFX per la gestione delle componenti grafiche dell'OLED
 * 4. WiFiNINA per la gestione dell'antena WiFi
 * 
 * Inoltre ho scelto 100cm come valore standard della larghezza di una porta ma ovviamente può essere modificato
 * 
 * last update 07/10/2020
 */



#include <Ultrasonic.h>
#include <MQTT.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128,64,&Wire,4); //inizializzazione display

#define BROKER_IP    "mqtt.eclipse.org"
#define DEV_NAME     "mqttdevice"
#define MQTT_USER    ""
#define MQTT_PW      ""

const char ssid[] = "";
const char pass[] = "";

WiFiClient net;
MQTTClient client; 
Ultrasonic ultrasonic(0);


void connect() {
 Serial.print("checking wifi...");
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(1000);
 }
 Serial.print("\nconnecting...");
 while (!client.connect(DEV_NAME, MQTT_USER, MQTT_PW)) {
   Serial.print(".");
   delay(1000);
 }
 Serial.println("\nconnected!");
 //sottoscrizione ai due canali mqtt
 client.subscribe("/lavatricelove"); 
 client.subscribe("/lavatricelove1");
 
}

void messageReceived(String &topic, String &payload) {
//Se il topic corrisponde al nome del canale allora fai cose
if (topic == "/lavatricelove1") {
  //Scrittura su display oled dei valori ricevuti
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.cp437(true);

  display.setCursor(0,0);
  display.print(payload + " PEOPLE");//scrive il payload
  display.display();
  
  delay(300);
 
  Serial.println(payload);
 }
}


void setup() {
 Serial.begin(115200);
 WiFi.begin(ssid, pass);
 client.begin(BROKER_IP, 1883, net);
 client.onMessage(messageReceived);
 connect();

 if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
   while(true);
 }
 display.display();
 display.clearDisplay();
}

void loop() {
 client.loop();
 if (!client.connected()){ 
   connect();
 }

//lettura sensore ultrasonico
long rangeInCm; rangeInCm = ultrasonic.MeasureInCentimeters();
delay(150);

 if(rangeInCm < 100){//se ci sono meno di 100cm conta. 
   client.publish("/lavatricelove", "1"); //manda i dati a node-red via mqtt
   delay(300);
  }
}



 
