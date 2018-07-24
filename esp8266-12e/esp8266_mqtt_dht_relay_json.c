/*
  Projet d'apprentissage d'un objet connecté (IoT)  pour réaliser une sonde de température
  ESP8266 + DHT22 + LED + MQTT + Home-Assistant
  Projets DIY (http://www.projetsdiy.fr) - Mai 2016
  Licence : MIT
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"          // Librairie des capteurs DHT

#define wifi_ssid "JohnColtrane"
#define wifi_password "comoTeLlamas?"

#define mqtt_server "gui.uva.es"
#define mqtt_user "kave"      // if exist
#define mqtt_password "mosquitto_passw"  //idem

#define temperature_topic "greenhouse/sensor3/temperature"  //Topic temperature
#define humidity_topic "greenhouse/sensor3/humidity"        //Topic humidity
#define both_topic "greenhouse/sensor3/both"                //Topic both
#define relay_topic "greenhouse/relay4"

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;
long lastRecu = 0;
//long timeBetweenSend = 60 * 5; // in seconds
long timeBetweenSend = 5; // in seconds
bool debug = true;  //Display log message if True
String both = "";
char *clientName = "ESP8266-12e_3";
int sensor = 3;

#define DHTPIN D4    // DHT Pin
#define D2 16

// Un-comment you sensor
//#define DHTTYPE DHT11       // DHT 11
#define DHTTYPE DHT22         // DHT 22  (AM2302)
#define D2 16

// Create abjects
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  pinMode(D2,OUTPUT);                     //Pin 2 for LED
  setup_wifi();                           //Connect to Wifi network
  client.setServer(mqtt_server, 5578);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message
  dht.begin();
}
//Connexion to WiFi network
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

//Reconnexion
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
  // Send a message every minute
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
// D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
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
    digitalWrite(D2,HIGH);
  } else {
    digitalWrite(D2,LOW);
  }
}