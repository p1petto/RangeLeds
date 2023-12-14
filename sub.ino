#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "microtest";
const char *password = "froggy12";


const int led_pin_1 = D1;
const int led_pin_2 = D2;
const int led_pin_3 = D5;
const int led_pin_4 = D6;


const char* mqtt_broker = "broker.emqx.io";
const char* topic_ultrasonic = "esp8266/ultrasonic-topic";
const char* mqtt_username = "emqx2";
const char* mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void WIFIconnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi network");
}
void setup() {
  Serial.begin(9600);
  WIFIconnect();
  pinMode(led_pin_1, OUTPUT);
  pinMode(led_pin_2, OUTPUT);
  pinMode(led_pin_3, OUTPUT);
  pinMode(led_pin_4, OUTPUT);
  digitalWrite(led_pin_1, LOW);
  digitalWrite(led_pin_2, LOW);
  digitalWrite(led_pin_3, LOW);
  digitalWrite(led_pin_4, LOW);
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  connectMQTT();

  client.subscribe(topic_ultrasonic);
}

void connectMQTT() {
  int attempt = 0;
  while (!client.connected() && attempt < 5) {
    String client_id = "led2-client-";
    client_id += String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      client.subscribe(topic_ultrasonic);
    } else {
      Serial.print("Failed to connect to MQTT Broker, rc=");
      Serial.println(client.state());
      Serial.println("Retrying in 2 seconds...");
      delay(2000);
      attempt++;
    }
  }
}

void loop() {
  client.loop();
}
void callback(char *topic, byte *payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Received message: ");
  Serial.println(message);

  float distance = message.toFloat();

  displayDistance(distance);
}

void displayDistance(float distance) {

  Serial.println(distance);
  if (distance < 50) {
    digitalWrite(led_pin_1, HIGH);
    digitalWrite(led_pin_2, LOW);
    digitalWrite(led_pin_3, LOW);
    digitalWrite(led_pin_4, LOW);
  }
   else if (distance >= 50 && distance < 100) {
    digitalWrite(led_pin_1, HIGH);
    digitalWrite(led_pin_2, HIGH);
    digitalWrite(led_pin_3, LOW);
    digitalWrite(led_pin_4, LOW);
  } 
  else if (distance >= 100 && distance < 150) {
    digitalWrite(led_pin_1, HIGH);
    digitalWrite(led_pin_2, HIGH);
    digitalWrite(led_pin_3, HIGH);
    digitalWrite(led_pin_4, LOW);
  } 
  else if (distance >= 150 ) {
    digitalWrite(led_pin_1, HIGH);
    digitalWrite(led_pin_2, HIGH);
    digitalWrite(led_pin_3, HIGH);
    digitalWrite(led_pin_4, HIGH);
  } 
}

