/*
   SMART P. COUNTER 2.0
   Il progetto si pone come obiettivo quello di contare le persone in ingresso e in uscita
   da un locale.
   Per fare ciò ho utilizzato un sensore ultrasonico installato su una porta, ogni volta che una persona passa davanti
   al sensore viene inviato un "1" via mqtt a node-red che a sua volta aggiornerà un contatore
   Inoltre node-red invia a sua volta il totale del contatore, sempre via mqtt, a uno schermo OLED

   N.B.
   Per creare questo progetto ho utilizzato le seguenti librerie:
   1. Grove Ultrasonic Ranger per la gestione del sensore ultrasonico
   2. u8g2 per la gestione dello schermo OLED
   4. WiFiNINA per la gestione dell'antena WiFi

   Inoltre ho scelto 100cm come valore standard della larghezza di una porta ma ovviamente può essere modificato


   last update 07/10/2020
*/


#include <Ultrasonic.h>
#include <MQTT.h>
#include <WiFiNINA.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

u8g2_uint_t offset;
u8g2_uint_t width;

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
    //scrittura su display
    do {
      u8g2.setFont(u8g2_font_inb16_mr);   // set font
      u8g2.setCursor(0, 20); //set cursore
      u8g2.println(payload + " PPL");//scrittura su display
      
    } while (u8g2.nextPage());//continua
    Serial.println(payload);
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  client.begin(BROKER_IP, 1883, net);
  //client.onMessage(messageReceived);
  connect();

  u8g2.begin();

}

void loop() {
  client.loop();
  if (!client.connected()) {
    connect();
  }

  //lettura sensore ultrasonico
  long rangeInCm; 
  rangeInCm = ultrasonic.MeasureInCentimeters();
  delay(150);

  if (rangeInCm < 100) { //se ci sono meno di 100cm conta.
    client.publish("/lavatricelove", "1"); //manda i dati a node-red via mqtt
    delay(300);
  }
}
