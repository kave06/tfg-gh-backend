/*
  Projet d'apprentissage d'un objet connecté (IoT)  pour réaliser une sonde de température
  ESP8266 + DHT22 + LED + MQTT + Home-Assistant
  Projets DIY (http://www.projetsdiy.fr) - Mai 2016
  Licence : MIT
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <EEPROM.h>

#define wifi_ssid "JohnColtrane"
#define wifi_password "comoTeLlamas?"

#define mqtt_server "gui.uva.es"
#define mqtt_user "kave"      // if exist
#define mqtt_password "mosquitto_passw"  //idem

#define flowMeter_topic "greenhouse/sensor4/flow"

//Buffer to decode MQTT messages
char message_buff[100];

long lastMsg = 0;
long lastSend = millis();
long now = 0;
boolean sendZero = false;

bool debug = true;  //Display log message if True
char *clientName = "ESP8266-12e_4";
int sensor = 4;

#define FLOWPIN 4    // Flow meter Pin

int counter = 0;
int liters = 0;
char litersString[10] = "0";
int addr = 0; // Set up eeprom

String one = "{\"sensor\"";
String two = "\"liters\"";
String two_points = ":";
String coma = ", ";

String send_tmp = one + two_points + sensor + coma + two + two_points;
String send_data = "";

// Create abjects
//DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  delay(10);
  pinMode(D2, OUTPUT);                    //Pin 2 for LED
  setup_wifi();                           //Connect to Wifi network
  client.setServer(mqtt_server, 5578);    // Configure MQTT connexion
  client.setCallback(callback);           // callback function to execute when a MQTT message
  pinMode(FLOWPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOWPIN), flow_ISR, FALLING);
  client.setCallback(callback);
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

//Reconnection
void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect(clientName, mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("KO, error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 secondes before to retry");
      delay(5000);
    }
  }
}
/********************************************************************************/
/**************************** loop **********************************************/
/********************************************************************************/
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  now = millis();
//  lastSend = now;

  if (counter > 440){
    if (debug){
      Serial.println (liters);
      send_data = send_tmp + String(liters) + "}";
      Serial.println(send_data);
    }
    liters += 1;
    counter = 0;
    send_data = send_tmp + String(liters) + "}";
    client.publish(flowMeter_topic, send_data.c_str(), true);
    sendZero = false;
  }

if (now - lastSend > 1000 * 5 && !sendZero) {
  liters = 0;
  send_data = send_tmp + String(liters) + "}";
  client.publish(flowMeter_topic, send_data.c_str(), true);
  if (debug){
    Serial.println(liters);
  }
  sendZero = true;
  }
}


/**********************************************************************/
// MQTT callback function
// D'après http://m2mio.tumblr.com/post/30048662088/a-simple-example-arduino-mqtt-m2mio
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | size: " + String(length, DEC));
  }
  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
}
/**********************************************************************/

void flow_ISR() {
  counter += 1;
//  stopFlow = false;
  lastSend = now;
}
