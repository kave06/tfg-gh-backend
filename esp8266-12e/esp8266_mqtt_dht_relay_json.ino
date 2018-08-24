/*
 Author Kave Heidarieh Sorosh

 This program is a part of an End of Degree Project developed already, that
 controls a greenhouse with IoT devices using MQTT protocol and an APIRest.
 For further information about the code, check the URLs shown bellow:
 https://github.com/kave06/tfg-gh-backend/tree/develop
 https://github.com/kave06/tfg-gh-tools/tree/develop
 url con API

This programm is based on a project that have MIT licence:
https://projetsdiy.fr/esp8266-dht22-mqtt-projet-objet-connecte/

Licence: GNU GPLv3

*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"          

#define wifi_ssid "xxxxxxxxxx"
#define wifi_password "xxxxxxxxx"

#define mqtt_server "xxxxxxxxxx"
#define mqtt_user "xxxxxxxxxx"
#define mqtt_password "xxxxxxxxxx"

/********************** Change number of sensor and clietName *******************/
int sensor = 3;
#define temperature_topic "greenhouse/sensor" + String(sensor)  + "/temperature"  //Topic temperature
#define humidity_topic "greenhouse/sensor3/humidity"        //Topic humidity
#define both_topic "greenhouse/sensor3/both"                //Topic both
#define relay_topic "greenhouse/relay4"                     //Topic of relay
char *clientName = "ESP8266-12e_3";
/********************************************************************************/

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;
long lastRecu = 0;
long timeBetweenSend = 60 * 5;  // in seconds
bool debug = true;              //Display log message if True
String both = "";


// Un-comment you sensor
#define DHTTYPE DHT22
#define DHTPIN D4               // DHT Pin
#define D0 16                   // Relay Pin

// Create abjects
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  pinMode(D0,OUTPUT);
  setup_wifi();                           //Connect to Wifi network
  client.setServer(mqtt_server, 5578);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message
  dht.begin();
}
//Connetion to WiFi network
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
  Serial.print(WiFi.localIP());
}

//Reconnetion
void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // Send a message every ...
  if (now - lastMsg > 1000 * timeBetweenSend) {
    lastMsg = now;
    // Read humidity
    float h = dht.readHumidity();
    // Read temperature in Celcius
    float t = dht.readTemperature();
    both = String(t) + " " + String(h);
    String one = "{\"sensor\"";
    String two = "\"temp\"";
    String three = "\"humi\"";
    String two_points = ":";
    String coma = ", ";
    String send_both = one + two_points + String(sensor) + coma + two + two_points + String(t) + coma + three + two_points + String(h) + "}";


    // Oh, nothing to send
    if ( isnan(t) || isnan(h)) {
      Serial.println("KO, Please check DHT sensor !");
      // return to setup()
      return;
    }

    if ( debug ) {
      Serial.print("Temperature : ");
      Serial.print(t);
      Serial.print(" | Humidity : ");
      Serial.println(h);
      Serial.println("\t" + both);
    }
    client.publish(temperature_topic, String(t).c_str(), true);   // Publish temperature on temperature_topic
    client.publish(humidity_topic, String(h).c_str(), true);      // and humidity
    client.publish(both_topic, send_both.c_str(), true);               // temperature and humidity
  }
  if (now - lastRecu > 100 ) {
    lastRecu = now;
    client.subscribe(relay_topic);
  }
}

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {

  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length,DEC));
  }
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }

  if ( msgString == "ON" ) {
    digitalWrite(D0,HIGH);
  } else {
    digitalWrite(D0,LOW);
  }
}
